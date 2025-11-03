#ifndef RENDER_H
#define RENDER_H

#include <stddef.h>

typedef struct {
    char *html;
    size_t len;
} render_result_t;

render_result_t *render_template(const char *template_name, void *data);
render_result_t *render_html(const char *html, size_t len);
void render_free(render_result_t *result);

char *render_escape_html(const char *str);

#endif
