#ifndef THREAD_H
#define THREAD_H

#include "http.h"

void thread_register_routes(void);
http_response_t *thread_view_handler(http_request_t *req);
http_response_t *thread_create_handler(http_request_t *req);

#endif
