#include "render.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

render_result_t *render_template(const char *template_name, void *data) {
    printf("Rendering template: %s (stub)\n", template_name);
    
    const char *html = "<html><body>Template stub</body></html>";
    return render_html(html, strlen(html));
}

render_result_t *render_html(const char *html, size_t len) {
    render_result_t *result = malloc(sizeof(render_result_t));
    if (!result) {
        return NULL;
    }
    
    result->html = malloc(len + 1);
    if (!result->html) {
        free(result);
        return NULL;
    }
    
    memcpy(result->html, html, len);
    result->html[len] = '\0';
    result->len = len;
    
    return result;
}

void render_free(render_result_t *result) {
    if (result) {
        if (result->html) {
            free(result->html);
        }
        free(result);
    }
}

char *render_escape_html(const char *str) {
    if (!str) {
        return NULL;
    }
    
    size_t len = strlen(str);
    char *escaped = malloc(len * 6 + 1);
    if (!escaped) {
        return NULL;
    }
    
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        switch (str[i]) {
            case '&':
                escaped[j++] = '&'; escaped[j++] = 'a'; escaped[j++] = 'm';
                escaped[j++] = 'p'; escaped[j++] = ';';
                break;
            case '<':
                escaped[j++] = '&'; escaped[j++] = 'l'; escaped[j++] = 't';
                escaped[j++] = ';';
                break;
            case '>':
                escaped[j++] = '&'; escaped[j++] = 'g'; escaped[j++] = 't';
                escaped[j++] = ';';
                break;
            case '"':
                escaped[j++] = '&'; escaped[j++] = 'q'; escaped[j++] = 'u';
                escaped[j++] = 'o'; escaped[j++] = 't'; escaped[j++] = ';';
                break;
            case '\'':
                escaped[j++] = '&'; escaped[j++] = '#'; escaped[j++] = '3';
                escaped[j++] = '9'; escaped[j++] = ';';
                break;
            default:
                escaped[j++] = str[i];
                break;
        }
    }
    escaped[j] = '\0';
    
    return escaped;
}
