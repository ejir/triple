#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static int hex_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

void url_decode(char *dst, const char *src, size_t dst_size) {
    size_t i = 0, j = 0;
    while (src[i] && j < dst_size - 1) {
        if (src[i] == '%' && src[i+1] && src[i+2]) {
            int high = hex_to_int(src[i+1]);
            int low = hex_to_int(src[i+2]);
            dst[j++] = (char)((high << 4) | low);
            i += 3;
        } else if (src[i] == '+') {
            dst[j++] = ' ';
            i++;
        } else {
            dst[j++] = src[i++];
        }
    }
    dst[j] = '\0';
}

char *get_cookie_value(const char *cookies, const char *name) {
    if (!cookies || !name) {
        return NULL;
    }
    
    static char value[256];
    char search_name[64];
    snprintf(search_name, sizeof(search_name), "%s=", name);
    
    const char *start = strstr(cookies, search_name);
    if (!start) {
        return NULL;
    }
    
    start += strlen(search_name);
    const char *end = strchr(start, ';');
    size_t len;
    
    if (end) {
        len = end - start;
    } else {
        len = strlen(start);
    }
    
    if (len >= sizeof(value)) {
        len = sizeof(value) - 1;
    }
    
    memcpy(value, start, len);
    value[len] = '\0';
    
    return value;
}

char *generate_random_token(int length) {
    static char token[256];
    static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    
    if (length >= 256) {
        length = 255;
    }
    
    srand(time(NULL));
    
    for (int i = 0; i < length; i++) {
        token[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    token[length] = '\0';
    
    return token;
}
