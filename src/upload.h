#ifndef UPLOAD_H
#define UPLOAD_H

#include "http.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
    char *filename;
    char *content_type;
    uint8_t *data;
    size_t size;
} upload_file_t;

void upload_init(const char *upload_dir);
void upload_register_routes(void);

http_response_t *upload_handler(http_request_t *req);
upload_file_t *upload_parse_multipart(http_request_t *req);
int upload_save_file(upload_file_t *file, const char *save_path);
void upload_file_free(upload_file_t *file);

#endif
