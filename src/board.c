#include "board.h"
#include "router.h"
#include "render.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void board_init(void) {
    printf("Board module initialized\n");
}

void board_register_routes(void) {
    router_add_route("GET", "/", board_list_handler);
    router_add_route("GET", "/board", board_view_handler);
    router_add_route("GET", "/thread", thread_view_handler);
    router_add_route("POST", "/thread", thread_create_handler);
    router_add_route("POST", "/post", post_create_handler);
}

http_response_t *board_list_handler(http_request_t *req) {
    const char *html = "<html><body><h1>Board List (stub)</h1></body></html>";
    return http_response_create(200, "text/html", html, strlen(html));
}

http_response_t *board_view_handler(http_request_t *req) {
    const char *html = "<html><body><h1>Board View (stub)</h1></body></html>";
    return http_response_create(200, "text/html", html, strlen(html));
}

http_response_t *thread_view_handler(http_request_t *req) {
    const char *html = "<html><body><h1>Thread View (stub)</h1></body></html>";
    return http_response_create(200, "text/html", html, strlen(html));
}

http_response_t *thread_create_handler(http_request_t *req) {
    const char *html = "<html><body><h1>Thread Created (stub)</h1></body></html>";
    return http_response_create(200, "text/html", html, strlen(html));
}

http_response_t *post_create_handler(http_request_t *req) {
    const char *html = "<html><body><h1>Post Created (stub)</h1></body></html>";
    return http_response_create(200, "text/html", html, strlen(html));
}

board_t *board_get_by_id(int64_t id) {
    printf("Getting board by ID: %lld (stub)\n", (long long)id);
    return NULL;
}

thread_t *thread_get_by_id(int64_t id) {
    printf("Getting thread by ID: %lld (stub)\n", (long long)id);
    return NULL;
}

void board_free(board_t *board) {
    if (board) {
        free(board->name);
        free(board->description);
        free(board);
    }
}

void thread_free(thread_t *thread) {
    if (thread) {
        free(thread->subject);
        free(thread->content);
        free(thread->author);
        free(thread);
    }
}

void post_free(post_t *post) {
    if (post) {
        free(post->content);
        free(post->author);
        free(post);
    }
}
