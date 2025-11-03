#ifndef ROUTER_H
#define ROUTER_H

#include "http.h"

typedef http_response_t *(*route_handler_t)(http_request_t *req);

typedef struct {
    const char *method;
    const char *path;
    route_handler_t handler;
} route_t;

void router_init(void);
void router_add_route(const char *method, const char *path, route_handler_t handler);
http_response_t *router_dispatch(http_request_t *req);
void router_cleanup(void);

#endif
