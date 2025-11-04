#ifndef POST_H
#define POST_H

#include "http.h"

void post_register_routes(void);
http_response_t *post_create_handler(http_request_t *req);

#endif
