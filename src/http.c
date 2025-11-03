#include "http.h"
#include "router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define BUFFER_SIZE 8192
#define BACKLOG 10

static int server_fd = -1;
static uint16_t server_port = 0;

static const char *get_status_message(int status_code) {
    switch (status_code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 304: return "Not Modified";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 500: return "Internal Server Error";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        default: return "Unknown";
    }
}

int http_server_init(uint16_t port) {
    struct sockaddr_in addr;
    int opt = 1;
    
    server_port = port;
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        fprintf(stderr, "Failed to create socket: %s\n", strerror(errno));
        return -1;
    }
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        fprintf(stderr, "Failed to set socket options: %s\n", strerror(errno));
        close(server_fd);
        server_fd = -1;
        return -1;
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "Failed to bind to port %u: %s\n", port, strerror(errno));
        close(server_fd);
        server_fd = -1;
        return -1;
    }
    
    if (listen(server_fd, BACKLOG) < 0) {
        fprintf(stderr, "Failed to listen: %s\n", strerror(errno));
        close(server_fd);
        server_fd = -1;
        return -1;
    }
    
    printf("HTTP server initialized on port %u\n", port);
    return 0;
}

static int parse_request_line(const char *line, http_request_t *req) {
    static char method[16];
    static char path[1024];
    static char query[1024];
    
    char *space1 = strchr(line, ' ');
    if (!space1) return -1;
    
    size_t method_len = space1 - line;
    if (method_len >= sizeof(method)) return -1;
    
    memcpy(method, line, method_len);
    method[method_len] = '\0';
    req->method = method;
    
    char *space2 = strchr(space1 + 1, ' ');
    if (!space2) return -1;
    
    size_t uri_len = space2 - (space1 + 1);
    if (uri_len >= sizeof(path)) return -1;
    
    char uri[1024];
    memcpy(uri, space1 + 1, uri_len);
    uri[uri_len] = '\0';
    
    char *question = strchr(uri, '?');
    if (question) {
        size_t path_len = question - uri;
        memcpy(path, uri, path_len);
        path[path_len] = '\0';
        strcpy(query, question + 1);
        req->query_string = query;
    } else {
        strcpy(path, uri);
        query[0] = '\0';
        req->query_string = NULL;
    }
    
    req->path = path;
    return 0;
}

static void send_response(int client_fd, http_response_t *response) {
    char header[1024];
    int header_len;
    
    const char *status_msg = get_status_message(response->status_code);
    
    header_len = snprintf(header, sizeof(header),
                         "HTTP/1.1 %d %s\r\n"
                         "Content-Type: %s\r\n"
                         "Content-Length: %zu\r\n"
                         "Connection: close\r\n"
                         "\r\n",
                         response->status_code,
                         status_msg,
                         response->content_type ? response->content_type : "text/plain",
                         response->body_len);
    
    write(client_fd, header, header_len);
    
    if (response->body && response->body_len > 0) {
        write(client_fd, response->body, response->body_len);
    }
}

static void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    http_request_t req;
    http_response_t *response;
    
    memset(&req, 0, sizeof(req));
    memset(buffer, 0, sizeof(buffer));
    
    bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        close(client_fd);
        return;
    }
    
    buffer[bytes_read] = '\0';
    
    char *line_end = strstr(buffer, "\r\n");
    if (!line_end) {
        const char *bad_request = "400 Bad Request";
        response = http_response_create(400, "text/plain", bad_request, strlen(bad_request));
        send_response(client_fd, response);
        http_response_free(response);
        close(client_fd);
        return;
    }
    
    *line_end = '\0';
    
    if (parse_request_line(buffer, &req) < 0) {
        const char *bad_request = "400 Bad Request";
        response = http_response_create(400, "text/plain", bad_request, strlen(bad_request));
        send_response(client_fd, response);
        http_response_free(response);
        close(client_fd);
        return;
    }
    
    char *headers_start = line_end + 2;
    char *body_start = strstr(headers_start, "\r\n\r\n");
    
    if (body_start) {
        body_start += 4;
        size_t body_len = bytes_read - (body_start - buffer);
        req.body = body_start;
        req.body_len = body_len;
        
        char *content_type = strstr(headers_start, "Content-Type:");
        if (content_type && content_type < body_start) {
            content_type += 13;
            while (*content_type == ' ') content_type++;
            char *type_end = strstr(content_type, "\r\n");
            if (type_end) {
                static char ct_buffer[128];
                size_t ct_len = type_end - content_type;
                if (ct_len < sizeof(ct_buffer)) {
                    memcpy(ct_buffer, content_type, ct_len);
                    ct_buffer[ct_len] = '\0';
                    req.content_type = ct_buffer;
                }
            }
        }
    }
    
    response = router_dispatch(&req);
    
    if (response) {
        send_response(client_fd, response);
        http_response_free(response);
    }
    
    close(client_fd);
}

void http_server_run(volatile int *keep_running) {
    if (server_fd < 0) {
        fprintf(stderr, "Server not initialized\n");
        return;
    }
    
    printf("HTTP server running on port %u\n", server_port);
    printf("Press Ctrl+C to stop\n");
    
    while (*keep_running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd;
        
        fd_set read_fds;
        struct timeval tv;
        
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        
        int select_result = select(server_fd + 1, &read_fds, NULL, NULL, &tv);
        
        if (select_result < 0) {
            if (errno == EINTR) {
                continue;
            }
            fprintf(stderr, "select() error: %s\n", strerror(errno));
            break;
        }
        
        if (select_result == 0) {
            continue;
        }
        
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            fprintf(stderr, "accept() error: %s\n", strerror(errno));
            continue;
        }
        
        handle_client(client_fd);
    }
}

void http_server_shutdown(void) {
    if (server_fd >= 0) {
        close(server_fd);
        server_fd = -1;
    }
    printf("HTTP server shutdown\n");
}

http_response_t *http_response_create(int status_code, const char *content_type, const char *body, size_t body_len) {
    http_response_t *response = malloc(sizeof(http_response_t));
    if (!response) {
        return NULL;
    }
    
    response->status_code = status_code;
    response->content_type = content_type;
    
    if (body && body_len > 0) {
        response->body = malloc(body_len);
        if (response->body) {
            memcpy(response->body, body, body_len);
            response->body_len = body_len;
        } else {
            free(response);
            return NULL;
        }
    } else {
        response->body = NULL;
        response->body_len = 0;
    }
    
    return response;
}

void http_response_free(http_response_t *response) {
    if (response) {
        if (response->body) {
            free(response->body);
        }
        free(response);
    }
}
