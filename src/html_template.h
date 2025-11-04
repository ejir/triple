#ifndef HTML_TEMPLATE_H
#define HTML_TEMPLATE_H

#include "i18n.h"

void html_render_header(char *buffer, int buffer_size, int *offset, 
                        const char *title, language_t lang);
void html_render_footer(char *buffer, int buffer_size, int *offset);
void html_render_nav_link(char *buffer, int buffer_size, int *offset,
                          const char *url, const char *text);
const char *html_get_common_css(void);

#endif
