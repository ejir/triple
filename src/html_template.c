#include "html_template.h"
#include <stdio.h>

const char *html_get_common_css(void) {
    return
        ":root {\n"
        "  --primary: #1976d2;\n"
        "  --primary-dark: #1565c0;\n"
        "  --primary-light: #42a5f5;\n"
        "  --accent: #ff4081;\n"
        "  --text-primary: rgba(0,0,0,0.87);\n"
        "  --text-secondary: rgba(0,0,0,0.54);\n"
        "  --divider: rgba(0,0,0,0.12);\n"
        "  --background: #fafafa;\n"
        "  --surface: #ffffff;\n"
        "  --error: #f44336;\n"
        "}\n"
        "* { box-sizing: border-box; margin: 0; padding: 0; }\n"
        "body {\n"
        "  font-family: 'Roboto', 'Segoe UI', Arial, sans-serif, 'Microsoft YaHei', 'SimHei';\n"
        "  background: var(--background);\n"
        "  color: var(--text-primary);\n"
        "  line-height: 1.6;\n"
        "  padding: 16px;\n"
        "}\n"
        ".container { max-width: 1200px; margin: 0 auto; }\n"
        ".card {\n"
        "  background: var(--surface);\n"
        "  border-radius: 4px;\n"
        "  box-shadow: 0 2px 4px rgba(0,0,0,0.1);\n"
        "  padding: 16px;\n"
        "  margin-bottom: 16px;\n"
        "  transition: box-shadow 0.3s;\n"
        "}\n"
        ".card:hover { box-shadow: 0 4px 8px rgba(0,0,0,0.15); }\n"
        ".header-card {\n"
        "  background: linear-gradient(135deg, var(--primary) 0%%, var(--primary-dark) 100%%);\n"
        "  color: white;\n"
        "  padding: 24px;\n"
        "  margin-bottom: 24px;\n"
        "  border-radius: 4px;\n"
        "}\n"
        ".header-card h1 { color: white; font-size: 2rem; font-weight: 500; margin-bottom: 8px; }\n"
        ".header-card p { color: rgba(255,255,255,0.9); font-size: 1rem; }\n"
        "h1 {\n"
        "  font-size: 2rem;\n"
        "  font-weight: 500;\n"
        "  margin-bottom: 24px;\n"
        "  color: var(--primary);\n"
        "  display: flex;\n"
        "  justify-content: space-between;\n"
        "  align-items: center;\n"
        "  flex-wrap: wrap;\n"
        "}\n"
        "h2 { font-size: 1.5rem; font-weight: 500; margin-bottom: 16px; color: var(--text-primary); }\n"
        "@media (max-width: 768px) {\n"
        "  h1 { font-size: 1.5rem; }\n"
        "  .header-card h1 { font-size: 1.5rem; }\n"
        "}\n"
        ".lang-switch { font-size: 0.875rem; font-weight: normal; }\n"
        ".lang-switch a {\n"
        "  color: var(--primary);\n"
        "  text-decoration: none;\n"
        "  padding: 6px 12px;\n"
        "  border: 1px solid var(--primary);\n"
        "  border-radius: 4px;\n"
        "  margin-left: 8px;\n"
        "  transition: all 0.2s;\n"
        "}\n"
        ".lang-switch a:hover { background: var(--primary); color: white; }\n"
        ".lang-switch a.active { background: var(--primary); color: white; }\n"
        ".nav-link {\n"
        "  color: rgba(255,255,255,0.9);\n"
        "  text-decoration: none;\n"
        "  margin-right: 16px;\n"
        "  display: inline-block;\n"
        "  margin-top: 12px;\n"
        "  font-size: 0.875rem;\n"
        "  transition: color 0.2s;\n"
        "}\n"
        ".nav-link:hover { color: white; text-decoration: underline; }\n"
        ".btn {\n"
        "  background: var(--primary);\n"
        "  color: white;\n"
        "  border: none;\n"
        "  padding: 10px 24px;\n"
        "  border-radius: 4px;\n"
        "  font-size: 0.875rem;\n"
        "  font-weight: 500;\n"
        "  text-transform: uppercase;\n"
        "  cursor: pointer;\n"
        "  box-shadow: 0 2px 4px rgba(0,0,0,0.2);\n"
        "  transition: all 0.2s;\n"
        "  min-height: 48px;\n"
        "}\n"
        ".btn:hover { background: var(--primary-dark); box-shadow: 0 4px 8px rgba(0,0,0,0.3); }\n"
        ".btn:active { box-shadow: 0 1px 2px rgba(0,0,0,0.2); }\n"
        "@media (max-width: 768px) { .btn { width: 100%%; } }\n"
        "input[type=\"text\"], input[type=\"password\"], textarea {\n"
        "  width: 100%%;\n"
        "  padding: 12px 16px;\n"
        "  margin: 8px 0;\n"
        "  border: 1px solid var(--divider);\n"
        "  border-radius: 4px;\n"
        "  font-size: 1rem;\n"
        "  font-family: inherit;\n"
        "  background: var(--surface);\n"
        "  transition: border-color 0.2s;\n"
        "}\n"
        "input:focus, textarea:focus {\n"
        "  outline: none;\n"
        "  border-color: var(--primary);\n"
        "  box-shadow: 0 0 0 2px rgba(25,118,210,0.1);\n"
        "}\n"
        "textarea { min-height: 120px; resize: vertical; }\n";
}

void html_render_header(char *buffer, int buffer_size, int *offset,
                        const char *title, language_t lang) {
    int len = *offset;
    
    len += snprintf(buffer + len, buffer_size - len,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<meta charset=\"UTF-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "<title>%s</title>\n"
        "<style>\n%s</style>\n"
        "<script>\n"
        "function setLanguage(lang) {\n"
        "  document.cookie = 'lang=' + lang + '; path=/; max-age=31536000';\n"
        "  window.location.href = window.location.pathname + '?lang=' + lang;\n"
        "}\n"
        "</script>\n"
        "</head>\n"
        "<body>\n"
        "<div class=\"container\">\n",
        title, html_get_common_css());
    
    *offset = len;
}

void html_render_footer(char *buffer, int buffer_size, int *offset) {
    int len = *offset;
    
    len += snprintf(buffer + len, buffer_size - len,
        "</div>\n"
        "</body>\n"
        "</html>");
    
    *offset = len;
}

void html_render_nav_link(char *buffer, int buffer_size, int *offset,
                          const char *url, const char *text) {
    int len = *offset;
    
    len += snprintf(buffer + len, buffer_size - len,
        "<a href=\"%s\" class=\"nav-link\">%s</a>\n",
        url, text);
    
    *offset = len;
}
