#include "upload.h"
#include "router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char upload_directory[256] = "./uploads";

void upload_init(const char *upload_dir) {
    if (upload_dir) {
        strncpy(upload_directory, upload_dir, sizeof(upload_directory) - 1);
        upload_directory[sizeof(upload_directory) - 1] = '\0';
    }
    printf("Upload module initialized, directory: %s\n", upload_directory);
}

void upload_register_routes(void) {
    router_add_route("POST", "/upload", upload_handler);
}

http_response_t *upload_handler(http_request_t *req) {
    const char *html = "<html><body><h1>File Upload (stub)</h1></body></html>";
    return http_response_create(200, "text/html", html, strlen(html));
}

upload_file_t *upload_parse_multipart(http_request_t *req) {
    printf("Parsing multipart upload (stub)\n");
    return NULL;
}

int upload_save_file(upload_file_t *file, const char *save_path) {
    printf("Saving file to: %s (stub)\n", save_path);
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
