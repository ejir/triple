#include "admin.h"
#include "router.h"
#include "render.h"
#include <stdio.h>
#include <string.h>

void admin_init(void) {
    printf("Admin module initialized\n");
}

void admin_register_routes(void) {
    router_add_route("GET", "/admin", admin_dashboard_handler);
    router_add_route("GET", "/admin/login", admin_login_handler);
    router_add_route("POST", "/admin/login", admin_login_handler);
    router_add_route("GET", "/admin/logout", admin_logout_handler);
}

http_response_t *admin_dashboard_handler(http_request_t *req) {
    const char *html = "<html><body><h1>Admin Dashboard (stub)</h1></body></html>";
    return http_response_create(200, "text/html", html, strlen(html));
}

http_response_t *admin_login_handler(http_request_t *req) {
    const char *html = "<html><body><h1>Admin Login (stub)</h1></body></html>";
    return http_response_create(200, "text/html", html, strlen(html));
}

http_response_t *admin_logout_handler(http_request_t *req) {
    const char *html = "<html><body><h1>Logged out (stub)</h1></body></html>";
    return http_response_create(200, "text/html", html, strlen(html));
}
