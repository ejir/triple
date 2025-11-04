#define _POSIX_C_SOURCE 200809L
#include "upload.h"
#include "router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

static char upload_directory[256] = "./uploads";

void upload_init(const char *upload_dir) {
    if (upload_dir) {
        strncpy(upload_directory, upload_dir, sizeof(upload_directory) - 1);
        upload_directory[sizeof(upload_directory) - 1] = '\0';
    }
    
    struct stat st;
    if (stat(upload_directory, &st) != 0) {
        if (mkdir(upload_directory, 0755) != 0) {
            fprintf(stderr, "Warning: Failed to create upload directory %s: %s\n", 
                    upload_directory, strerror(errno));
        } else {
            printf("Created upload directory: %s\n", upload_directory);
        }
    }
    
    printf("Upload module initialized, directory: %s\n", upload_directory);
}

void upload_register_routes(void) {
    router_add_route("GET", "/upload", upload_handler);
    router_add_route("POST", "/upload", upload_handler);
}

http_response_t *upload_handler(http_request_t *req) {
    if (strcmp(req->method, "GET") == 0) {
        const char *html = 
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head><title>File Upload</title></head>\n"
            "<body>\n"
            "<h1>File Upload</h1>\n"
            "<form method=\"POST\" enctype=\"multipart/form-data\">\n"
            "<input type=\"file\" name=\"file\" required><br>\n"
            "<button type=\"submit\">Upload</button>\n"
            "</form>\n"
            "<hr>\n"
            "<a href=\"/\">Back to Home</a>\n"
            "</body>\n"
            "</html>";
        return http_response_create(200, "text/html", html, strlen(html));
    }
    
    upload_file_t *file = upload_parse_multipart(req);
    if (!file) {
        const char *html = "<html><body><h1>Upload Failed</h1><p>No file data received</p><a href=\"/upload\">Try Again</a></body></html>";
        return http_response_create(400, "text/html", html, strlen(html));
    }
    
    char save_path[512];
    snprintf(save_path, sizeof(save_path), "%s/%s", upload_directory, file->filename);
    
    int result = upload_save_file(file, save_path);
    upload_file_free(file);
    
    char *html = malloc(1024);
    if (!html) {
        const char *err = "<html><body><h1>Error: Out of memory</h1></body></html>";
        return http_response_create(500, "text/html", err, strlen(err));
    }
    
    if (result == 0) {
        snprintf(html, 1024,
            "<html><body><h1>Upload Successful</h1>"
            "<p>File saved to: %s</p>"
            "<a href=\"/upload\">Upload Another</a> | "
            "<a href=\"/\">Home</a></body></html>",
            save_path);
        http_response_t *response = http_response_create(200, "text/html", html, strlen(html));
        free(html);
        return response;
    } else {
        snprintf(html, 1024,
            "<html><body><h1>Upload Failed</h1>"
            "<p>Failed to save file</p>"
            "<a href=\"/upload\">Try Again</a></body></html>");
        http_response_t *response = http_response_create(500, "text/html", html, strlen(html));
        free(html);
        return response;
    }
}

upload_file_t *upload_parse_multipart(http_request_t *req) {
    printf("Parsing multipart upload (basic implementation)\n");
    
    if (!req->body || req->body_len == 0) {
        return NULL;
    }
    
    upload_file_t *file = malloc(sizeof(upload_file_t));
    if (!file) {
        return NULL;
    }
    
    file->filename = strdup("uploaded_file.dat");
    file->content_type = strdup("application/octet-stream");
    file->size = req->body_len;
    file->data = malloc(file->size);
    
    if (!file->data) {
        upload_file_free(file);
        return NULL;
    }
    
    memcpy(file->data, req->body, file->size);
    return file;
}

int upload_save_file(upload_file_t *file, const char *save_path) {
    if (!file || !file->data || !save_path) {
        return -1;
    }
    
    FILE *fp = fopen(save_path, "wb");
    if (!fp) {
        fprintf(stderr, "Failed to open file for writing: %s\n", save_path);
        return -1;
    }
    
    size_t written = fwrite(file->data, 1, file->size, fp);
    fclose(fp);
    
    if (written != file->size) {
        fprintf(stderr, "Failed to write complete file\n");
        return -1;
    }
    
    printf("File saved successfully: %s (%zu bytes)\n", save_path, file->size);
    return 0;
}

void upload_file_free(upload_file_t *file) {
    if (file) {
        free(file->filename);
        free(file->content_type);
        free(file->data);
        free(file);
    }
}
