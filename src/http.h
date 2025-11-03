#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char *method;
    const char *path;
    const char *query_string;
    const char *body;
    size_t body_len;
    const char *content_type;
    const char *cookies;
} http_request_t;

typedef struct {
    int status_code;
    const char *content_type;
    char *body;
    size_t body_len;
    char *set_cookie;
} http_response_t;

int http_server_init(uint16_t port);
void http_server_run(volatile int *keep_running);
void http_server_shutdown(void);

http_response_t *http_response_create(int status_code, const char *content_type, const char *body, size_t body_len);
void http_response_free(http_response_t *response);

#endif
