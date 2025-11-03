#ifndef BOARD_H
#define BOARD_H

#include "http.h"
#include <stdint.h>

typedef struct {
    int64_t id;
    char *name;
    char *description;
} board_t;

typedef struct {
    int64_t id;
    int64_t board_id;
    char *subject;
    char *content;
    char *author;
    int64_t created_at;
} thread_t;

typedef struct {
    int64_t id;
    int64_t thread_id;
    char *content;
    char *author;
    int64_t created_at;
} post_t;

void board_init(void);
void board_register_routes(void);

http_response_t *board_list_handler(http_request_t *req);
http_response_t *board_view_handler(http_request_t *req);
http_response_t *thread_view_handler(http_request_t *req);
http_response_t *thread_create_handler(http_request_t *req);
http_response_t *post_create_handler(http_request_t *req);

board_t *board_get_by_id(int64_t id);
thread_t *thread_get_by_id(int64_t id);
void board_free(board_t *board);
void thread_free(thread_t *thread);
void post_free(post_t *post);

#endif
