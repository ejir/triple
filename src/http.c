#define _POSIX_C_SOURCE 200809L
#include "http.h"
#include "router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int server_running = 0;
static uint16_t server_port = 0;

int http_server_init(uint16_t port) {
    server_port = port;
    printf("HTTP server initialized on port %u\n", port);
    return 0;
}

void http_server_run(volatile int *keep_running) {
    server_running = 1;
    printf("HTTP server running on port %u (stub implementation)\n", server_port);
    printf("Press Ctrl+C to stop\n");
    
    while (*keep_running) {
        // In a real implementation, this would handle HTTP requests
        // For now, just sleep to avoid busy waiting
        sleep(1);
    }
}

void http_server_shutdown(void) {
    server_running = 0;
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
