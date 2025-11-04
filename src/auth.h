#ifndef AUTH_H
#define AUTH_H

#include "http.h"

int auth_is_authenticated(http_request_t *req);
char *auth_create_session(int user_id);
void auth_destroy_session(const char *token);

#endif
