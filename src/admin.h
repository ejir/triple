#ifndef ADMIN_H
#define ADMIN_H

#include "http.h"

void admin_init(void);
void admin_register_routes(void);

int admin_is_authenticated(http_request_t *req);

http_response_t *admin_dashboard_handler(http_request_t *req);
http_response_t *admin_login_handler(http_request_t *req);
http_response_t *admin_logout_handler(http_request_t *req);
http_response_t *admin_change_password_handler(http_request_t *req);

#endif
