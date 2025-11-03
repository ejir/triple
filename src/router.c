#include "router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROUTES 64

static route_t routes[MAX_ROUTES];
static size_t route_count = 0;

void router_init(void) {
    route_count = 0;
    printf("Router initialized\n");
}

void router_add_route(const char *method, const char *path, route_handler_t handler) {
    if (route_count >= MAX_ROUTES) {
        fprintf(stderr, "Router: maximum routes exceeded\n");
        return;
    }
    
    routes[route_count].method = method;
    routes[route_count].path = path;
    routes[route_count].handler = handler;
    route_count++;
    
    printf("Route added: %s %s\n", method, path);
}

http_response_t *router_dispatch(http_request_t *req) {
    for (size_t i = 0; i < route_count; i++) {
        if (strcmp(req->method, routes[i].method) == 0 &&
            strcmp(req->path, routes[i].path) == 0) {
            return routes[i].handler(req);
        }
    }
    
    const char *not_found = "404 Not Found";
    return http_response_create(404, "text/plain", not_found, strlen(not_found));
}

void router_cleanup(void) {
    route_count = 0;
    printf("Router cleaned up\n");
}
