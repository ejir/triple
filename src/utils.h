#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

void url_decode(char *dst, const char *src, size_t dst_size);
char *get_cookie_value(const char *cookies, const char *name);
char *generate_random_token(int length);

#endif
