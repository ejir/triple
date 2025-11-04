#define _POSIX_C_SOURCE 200809L
#include "board.h"
#include "router.h"
#include "render.h"
#include "db.h"
#include "admin.h"
#include "i18n.h"
#include "kaomoji.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

void board_init(void) {
    printf("Board module initialized\n");
    
    sqlite3_stmt *stmt = db_prepare("SELECT COUNT(*) FROM boards");
    if (stmt) {
        if (db_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            if (count == 0) {
                printf("Creating sample boards...\n");
                db_exec("INSERT INTO boards (name, title, description) VALUES "
                       "('general', 'General Discussion', 'General discussion topics'), "
                       "('tech', 'Technology', 'Technology and programming discussions'), "
                       "('random', 'Random', 'Random and off-topic discussions')");
            }
        }
        db_finalize(stmt);
    }
}

void board_register_routes(void) {
    router_add_route("GET", "/", board_list_handler);
    router_add_route("GET", "/board", board_view_handler);
    router_add_route("POST", "/board/create", board_create_handler);
    router_add_route("GET", "/thread", thread_view_handler);
    router_add_route("POST", "/thread", thread_create_handler);
    router_add_route("POST", "/post", post_create_handler);
}

http_response_t *board_list_handler(http_request_t *req) {
    language_t lang = i18n_get_language(req);
    
    char *html = malloc(16384);
    if (!html) {
        const char *err = "<html><body><h1>Error: Out of memory</h1></body></html>";
        return http_response_create(500, "text/html", err, strlen(err));
    }
    
    int len = snprintf(html, 16384,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<meta charset=\"UTF-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "<title>%s</title>\n"
        "<style>\n"
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
        "}\n"
        ".card:hover { box-shadow: 0 4px 8px rgba(0,0,0,0.15); transition: box-shadow 0.3s; }\n"
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
        "@media (max-width: 768px) { h1 { font-size: 1.5rem; } }\n"
        ".lang-switch { font-size: 0.875rem; font-weight: normal; }\n"
        ".lang-switch a { color: var(--primary); text-decoration: none; padding: 6px 12px;\n"
        "  border: 1px solid var(--primary); border-radius: 4px; margin-left: 8px; transition: all 0.2s; }\n"
        ".lang-switch a:hover { background: var(--primary); color: white; }\n"
        ".lang-switch a.active { background: var(--primary); color: white; }\n"
        ".board-list { list-style: none; }\n"
        ".board-item {\n"
        "  display: block;\n"
        "  padding: 16px;\n"
        "  background: var(--surface);\n"
        "  border-radius: 4px;\n"
        "  margin-bottom: 12px;\n"
        "  box-shadow: 0 1px 3px rgba(0,0,0,0.1);\n"
        "  transition: all 0.2s;\n"
        "}\n"
        ".board-item:hover {\n"
        "  box-shadow: 0 4px 8px rgba(0,0,0,0.15);\n"
        "  transform: translateY(-2px);\n"
        "}\n"
        ".board-link {\n"
        "  color: var(--primary);\n"
        "  text-decoration: none;\n"
        "  font-size: 1.25rem;\n"
        "  font-weight: 500;\n"
        "  display: block;\n"
        "  margin-bottom: 8px;\n"
        "}\n"
        ".board-desc {\n"
        "  color: var(--text-secondary);\n"
        "  font-size: 0.875rem;\n"
        "  display: block;\n"
        "}\n"
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
        "input[type=\"text\"], textarea {\n"
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
        "textarea { min-height: 120px; resize: vertical; }\n"
        "</style>\n"
        "<script>\n"
        "function setLanguage(lang) {\n"
        "  document.cookie = 'lang=' + lang + '; path=/; max-age=31536000';\n"
        "  window.location.href = '/?lang=' + lang;\n"
        "}\n"
        "</script>\n"
        "</head>\n"
        "<body>\n"
        "<div class=\"container\">\n"
        "<h1>\n"
        "  <span>📋 %s</span>\n"
        "  <span class=\"lang-switch\">\n"
        "    <a href=\"#\" onclick=\"setLanguage('en'); return false;\" class=\"%s\">English</a>\n"
        "    <a href=\"#\" onclick=\"setLanguage('zh-cn'); return false;\" class=\"%s\">中文</a>\n"
        "  </span>\n"
        "</h1>\n"
        "<ul class=\"board-list\">\n",
        i18n_get(lang, "message_boards"),
        i18n_get(lang, "message_boards"),
        (lang == LANG_EN ? "active" : ""),
        (lang == LANG_ZH_CN ? "active" : ""));
    
    sqlite3_stmt *stmt = db_prepare("SELECT id, name, title, description FROM boards ORDER BY name");
    if (stmt) {
        while (db_step(stmt) == SQLITE_ROW) {
            int64_t id = sqlite3_column_int64(stmt, 0);
            const char *name = (const char *)sqlite3_column_text(stmt, 1);
            const char *title = (const char *)sqlite3_column_text(stmt, 2);
            const char *desc = (const char *)sqlite3_column_text(stmt, 3);
            
            char *escaped_name = render_escape_html(name ? name : "Unknown");
            char *escaped_title = render_escape_html(title ? title : "No Title");
            char *escaped_desc = render_escape_html(desc ? desc : "");
            
            len += snprintf(html + len, 8192 - len,
                "<li class=\"board-item\">\n"
                "<a href=\"/board?id=%lld\" class=\"board-link\">%s - %s</a>\n"
                "<span class=\"board-desc\">%s</span>\n"
                "</li>\n",
                (long long)id, 
                escaped_name ? escaped_name : "Unknown",
                escaped_title ? escaped_title : "No Title",
                escaped_desc ? escaped_desc : "");
            
            free(escaped_name);
            free(escaped_title);
            free(escaped_desc);
        }
        db_finalize(stmt);
    }
    
    len += snprintf(html + len, 16384 - len, "</ul>\n");
    
    if (admin_is_authenticated(req)) {
        len += snprintf(html + len, 16384 - len,
            "<div class=\"card\" style=\"margin-top:24px;\">\n"
            "<h2 style=\"font-size:1.5rem;margin-bottom:16px;\">%s</h2>\n"
            "<form method=\"POST\" action=\"/board/create\">\n"
            "<div style=\"margin-bottom:16px;\">\n"
            "<label style=\"display:block;margin-bottom:4px;font-weight:500;\">%s</label>\n"
            "<input type=\"text\" name=\"name\" required>\n"
            "</div>\n"
            "<div style=\"margin-bottom:16px;\">\n"
            "<label style=\"display:block;margin-bottom:4px;font-weight:500;\">%s</label>\n"
            "<input type=\"text\" name=\"title\" required>\n"
            "</div>\n"
            "<div style=\"margin-bottom:16px;\">\n"
            "<label style=\"display:block;margin-bottom:4px;font-weight:500;\">%s</label>\n"
            "<textarea name=\"description\"></textarea>\n"
            "</div>\n"
            "<button type=\"submit\" class=\"btn\">%s</button>\n"
            "</form>\n"
            "</div>\n",
            i18n_get(lang, "create_new_board"),
            i18n_get(lang, "name"),
            i18n_get(lang, "title"),
            i18n_get(lang, "description"),
            i18n_get(lang, "create_board"));
    }
    
    len += snprintf(html + len, 16384 - len,
        "</div>\n"
        "</body>\n"
        "</html>");
    
    http_response_t *response = http_response_create(200, "text/html", html, len);
    free(html);
    return response;
}

http_response_t *board_create_handler(http_request_t *req) {
    if (!admin_is_authenticated(req)) {
        const char *html = 
            "<html><body><h1>Access Denied</h1>"
            "<p>Only administrators can create boards.</p>"
            "<a href=\"/\">Back to boards</a></body></html>";
        return http_response_create(403, "text/html", html, strlen(html));
    }
    
    if (!req->body) {
        const char *html = "<html><body><h1>Error: No form data</h1></body></html>";
        return http_response_create(400, "text/html", html, strlen(html));
    }
    
    char name[256] = {0};
    char title[256] = {0};
    char description[1024] = {0};
    
    char *body_copy = strdup(req->body);
    if (!body_copy) {
        const char *html = "<html><body><h1>Error: Out of memory</h1></body></html>";
        return http_response_create(500, "text/html", html, strlen(html));
    }
    
    char *token = strtok(body_copy, "&");
    while (token) {
        char *eq = strchr(token, '=');
        if (eq) {
            *eq = '\0';
            char *key = token;
            char *value = eq + 1;
            
            if (strcmp(key, "name") == 0) {
                url_decode(name, value, sizeof(name));
            } else if (strcmp(key, "title") == 0) {
                url_decode(title, value, sizeof(title));
            } else if (strcmp(key, "description") == 0) {
                url_decode(description, value, sizeof(description));
            }
        }
        token = strtok(NULL, "&");
    }
    free(body_copy);
    
    if (strlen(name) == 0 || strlen(title) == 0) {
        const char *html = "<html><body><h1>Error: Name and title are required</h1>"
                          "<a href=\"/\">Back to boards</a></body></html>";
        return http_response_create(400, "text/html", html, strlen(html));
    }
    
    sqlite3_stmt *stmt = db_prepare(
        "INSERT INTO boards (name, title, description) VALUES (?, ?, ?)"
    );
    if (!stmt) {
        const char *html = "<html><body><h1>Error: Failed to create board</h1></body></html>";
        return http_response_create(500, "text/html", html, strlen(html));
    }
    
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, title, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, description, -1, SQLITE_TRANSIENT);
    
    int rc = db_step(stmt);
    int64_t board_id = sqlite3_last_insert_rowid(db_get_connection());
    db_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        const char *html = "<html><body><h1>Error: Failed to create board</h1></body></html>";
        return http_response_create(500, "text/html", html, strlen(html));
    }
    
    char *html = malloc(512);
    if (!html) {
        const char *err = "<html><body><h1>Error: Out of memory</h1></body></html>";
        return http_response_create(500, "text/html", err, strlen(err));
    }
    
    char *escaped_title = render_escape_html(title);
    
    int len = snprintf(html, 512,
        "<html><body><h1>Board Created!</h1>"
        "<p>Board '%s' has been created.</p>"
        "<a href=\"/board?id=%lld\">View Board</a> | "
        "<a href=\"/\">Back to All Boards</a></body></html>",
        escaped_title ? escaped_title : title,
        (long long)board_id);
    
    free(escaped_title);
    
    http_response_t *response = http_response_create(200, "text/html", html, len);
    free(html);
    return response;
}

http_response_t *board_view_handler(http_request_t *req) {
    language_t lang = i18n_get_language(req);
    
    int64_t board_id = 1;
    if (req->query_string) {
        sscanf(req->query_string, "id=%lld", (long long *)&board_id);
    }
    
    board_t *board = board_get_by_id(board_id);
    if (!board) {
        char error_html[512];
        snprintf(error_html, sizeof(error_html),
            "<html><body><h1>%s</h1><a href=\"/\">%s</a></body></html>",
            i18n_get(lang, "board_not_found"),
            i18n_get(lang, "back_to_boards"));
        return http_response_create(404, "text/html", error_html, strlen(error_html));
    }
    
    char *html = malloc(65536);
    if (!html) {
        board_free(board);
        const char *err = "<html><body><h1>Error: Out of memory</h1></body></html>";
        return http_response_create(500, "text/html", err, strlen(err));
    }
    
    char *escaped_name_title = render_escape_html(board->name ? board->name : "Board");
    char *escaped_name_h1 = render_escape_html(board->name ? board->name : "board");
    char *escaped_name_body = render_escape_html(board->name ? board->name : "Board");
    char *escaped_desc = render_escape_html(board->description ? board->description : "No description");
    
    int len = snprintf(html, 65536,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<meta charset=\"UTF-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "<title>%s</title>\n"
        "<style>\n"
        ":root {\n"
        "  --primary: #1976d2; --primary-dark: #1565c0; --primary-light: #42a5f5;\n"
        "  --accent: #ff4081; --text-primary: rgba(0,0,0,0.87); --text-secondary: rgba(0,0,0,0.54);\n"
        "  --divider: rgba(0,0,0,0.12); --background: #fafafa; --surface: #ffffff; --error: #f44336;\n"
        "}\n"
        "* { box-sizing: border-box; margin: 0; padding: 0; }\n"
        "body { font-family: 'Roboto', 'Segoe UI', Arial, sans-serif, 'Microsoft YaHei', 'SimHei'; background: var(--background);\n"
        "  color: var(--text-primary); line-height: 1.6; padding: 16px; }\n"
        ".container { max-width: 1200px; margin: 0 auto; }\n"
        ".card { background: var(--surface); border-radius: 4px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);\n"
        "  padding: 16px; margin-bottom: 16px; transition: box-shadow 0.3s; }\n"
        ".card:hover { box-shadow: 0 4px 8px rgba(0,0,0,0.15); }\n"
        ".header-card { background: linear-gradient(135deg, var(--primary) 0%%, var(--primary-dark) 100%%);\n"
        "  color: white; padding: 24px; margin-bottom: 24px; }\n"
        ".header-card h1 { color: white; font-size: 2rem; font-weight: 500; margin-bottom: 8px; }\n"
        ".header-card p { color: rgba(255,255,255,0.9); font-size: 1rem; }\n"
        "@media (max-width: 768px) { .header-card h1 { font-size: 1.5rem; } }\n"
        ".nav-link { color: rgba(255,255,255,0.9); text-decoration: none; margin-right: 16px;\n"
        "  display: inline-block; margin-top: 12px; font-size: 0.875rem; transition: color 0.2s; }\n"
        ".nav-link:hover { color: white; text-decoration: underline; }\n"
        "h2 { font-size: 1.5rem; font-weight: 500; margin-bottom: 16px; color: var(--text-primary); }\n"
        ".thread-list { list-style: none; }\n"
        ".thread-item { background: var(--surface); padding: 16px; margin-bottom: 12px;\n"
        "  border-radius: 4px; box-shadow: 0 1px 3px rgba(0,0,0,0.1); transition: all 0.2s;\n"
        "  display: flex; justify-content: space-between; align-items: center; }\n"
        ".thread-item:hover { box-shadow: 0 4px 8px rgba(0,0,0,0.15); transform: translateY(-2px); }\n"
        ".thread-link { color: var(--primary); text-decoration: none; font-size: 1.125rem;\n"
        "  font-weight: 500; flex: 1; }\n"
        ".thread-link:hover { text-decoration: underline; }\n"
        ".thread-meta { color: var(--text-secondary); font-size: 0.875rem; margin-left: 16px;\n"
        "  white-space: nowrap; }\n"
        "@media (max-width: 768px) {\n"
        "  .thread-item { flex-direction: column; align-items: flex-start; }\n"
        "  .thread-meta { margin-left: 0; margin-top: 8px; }\n"
        "}\n"
        ".btn { background: var(--primary); color: white; border: none; padding: 10px 24px;\n"
        "  border-radius: 4px; font-size: 0.875rem; font-weight: 500; text-transform: uppercase;\n"
        "  cursor: pointer; box-shadow: 0 2px 4px rgba(0,0,0,0.2); transition: all 0.2s; min-height: 48px; }\n"
        ".btn:hover { background: var(--primary-dark); box-shadow: 0 4px 8px rgba(0,0,0,0.3); }\n"
        ".btn:active { box-shadow: 0 1px 2px rgba(0,0,0,0.2); }\n"
        "@media (max-width: 768px) { .btn { width: 100%%; } }\n"
        "input[type=\"text\"], textarea { width: 100%%; padding: 12px 16px; margin: 8px 0;\n"
        "  border: 1px solid var(--divider); border-radius: 4px; font-size: 1rem; font-family: inherit;\n"
        "  background: var(--surface); transition: border-color 0.2s; }\n"
        "input:focus, textarea:focus { outline: none; border-color: var(--primary);\n"
        "  box-shadow: 0 0 0 2px rgba(25,118,210,0.1); }\n"
        "textarea { min-height: 120px; resize: vertical; }\n"
        "label { display: block; margin-bottom: 4px; font-weight: 500; color: var(--text-primary); }\n"
        ".form-group { margin-bottom: 16px; }\n"
        ".kaomoji-btn { background: var(--accent); color: white; border: none; padding: 8px 16px;\n"
        "  cursor: pointer; border-radius: 4px; font-weight: 500; transition: background 0.2s;\n"
        "  font-size: 0.875rem; box-shadow: 0 2px 4px rgba(0,0,0,0.2); }\n"
        ".kaomoji-btn:hover { background: #e91e63; box-shadow: 0 3px 6px rgba(0,0,0,0.3); }\n"
        ".kaomoji-modal { display: none; position: fixed; z-index: 1000; left: 0; top: 0;\n"
        "  width: 100%%; height: 100%%; background: rgba(0,0,0,0.5); align-items: center;\n"
        "  justify-content: center; }\n"
        ".kaomoji-modal.show { display: flex; }\n"
        ".kaomoji-popup { background: var(--surface); border-radius: 8px; box-shadow: 0 4px 20px rgba(0,0,0,0.3);\n"
        "  max-width: 600px; width: 90%%; max-height: 70vh; display: flex; flex-direction: column;\n"
        "  position: relative; }\n"
        ".kaomoji-header { display: flex; justify-content: space-between; align-items: center;\n"
        "  padding: 16px 20px; border-bottom: 1px solid var(--divider); }\n"
        ".kaomoji-title { font-size: 1.125rem; font-weight: 500; color: var(--text-primary); }\n"
        ".kaomoji-close { background: none; border: none; font-size: 1.5rem; color: var(--text-secondary);\n"
        "  cursor: pointer; padding: 0; width: 32px; height: 32px; border-radius: 50%%;\n"
        "  transition: all 0.2s; line-height: 1; }\n"
        ".kaomoji-close:hover { background: rgba(0,0,0,0.05); color: var(--text-primary); }\n"
        ".kaomoji-tabs { display: flex; overflow-x: auto; border-bottom: 1px solid var(--divider);\n"
        "  background: #f5f5f5; }\n"
        ".kaomoji-tab { background: none; border: none; padding: 12px 20px; cursor: pointer;\n"
        "  font-size: 0.875rem; font-weight: 500; color: var(--text-secondary);\n"
        "  transition: all 0.2s; white-space: nowrap; border-bottom: 2px solid transparent; }\n"
        ".kaomoji-tab:hover { background: rgba(25,118,210,0.05); color: var(--primary); }\n"
        ".kaomoji-tab.active { color: var(--primary); border-bottom-color: var(--primary); background: white; }\n"
        ".kaomoji-content { flex: 1; overflow-y: auto; padding: 16px 20px; }\n"
        ".kaomoji-category { display: none; }\n"
        ".kaomoji-category.active { display: block; }\n"
        ".kaomoji-items { display: flex; flex-wrap: wrap; gap: 8px; }\n"
        ".kaomoji-item { padding: 8px 16px; border: 1px solid var(--divider); background: white;\n"
        "  cursor: pointer; border-radius: 4px; font-size: 1rem; transition: all 0.2s;\n"
        "  box-shadow: 0 1px 2px rgba(0,0,0,0.05); }\n"
        ".kaomoji-item:hover { background: #e3f2fd; border-color: var(--primary);\n"
        "  box-shadow: 0 2px 4px rgba(0,0,0,0.15); transform: translateY(-1px); }\n"
        ".kaomoji-item:active { transform: translateY(0); box-shadow: 0 1px 2px rgba(0,0,0,0.1); }\n"
        "@media (max-width: 768px) {\n"
        "  .kaomoji-popup { width: 95%%; max-height: 80vh; }\n"
        "  .kaomoji-tab { padding: 10px 12px; font-size: 0.8rem; }\n"
        "  .kaomoji-item { padding: 6px 12px; font-size: 0.9rem; }\n"
        "}\n"
        "</style>\n"
        "<script>\n"
        "var currentTab = 0;\n"
        "function openKaomoji() {\n"
        "  document.getElementById('kaomoji-modal').classList.add('show');\n"
        "}\n"
        "function closeKaomoji() {\n"
        "  document.getElementById('kaomoji-modal').classList.remove('show');\n"
        "}\n"
        "function switchTab(index) {\n"
        "  currentTab = index;\n"
        "  var tabs = document.querySelectorAll('.kaomoji-tab');\n"
        "  var categories = document.querySelectorAll('.kaomoji-category');\n"
        "  tabs.forEach(function(tab, i) {\n"
        "    if (i === index) { tab.classList.add('active'); } else { tab.classList.remove('active'); }\n"
        "  });\n"
        "  categories.forEach(function(cat, i) {\n"
        "    if (i === index) { cat.classList.add('active'); } else { cat.classList.remove('active'); }\n"
        "  });\n"
        "}\n"
        "function insertKaomoji(kaomoji) {\n"
        "  var textarea = document.querySelector('textarea[name=\"content\"]');\n"
        "  var start = textarea.selectionStart;\n"
        "  var end = textarea.selectionEnd;\n"
        "  var text = textarea.value;\n"
        "  textarea.value = text.substring(0, start) + kaomoji + text.substring(end);\n"
        "  textarea.selectionStart = textarea.selectionEnd = start + kaomoji.length;\n"
        "  textarea.focus();\n"
        "  closeKaomoji();\n"
        "}\n"
        "window.onclick = function(event) {\n"
        "  var modal = document.getElementById('kaomoji-modal');\n"
        "  if (event.target === modal) { closeKaomoji(); }\n"
        "}\n"
        "function setLanguage(lang) {\n"
        "  document.cookie = 'lang=' + lang + '; path=/; max-age=31536000';\n"
        "  window.location.href = window.location.pathname + '?id=%lld&lang=' + lang;\n"
        "}\n"
        "</script>\n"
        "</head>\n"
        "<body>\n"
        "<div class=\"container\">\n"
        "<div class=\"header-card card\">\n"
        "<h1 style=\"display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;\">\n"
        "  <span>/%s/ - %s</span>\n"
        "  <span style=\"font-size:0.875rem;font-weight:normal;\">\n"
        "    <a href=\"#\" onclick=\"setLanguage('en'); return false;\" style=\"color:rgba(255,255,255,0.9);text-decoration:none;padding:6px 12px;border:1px solid rgba(255,255,255,0.5);border-radius:4px;margin-left:8px;%s\">English</a>\n"
        "    <a href=\"#\" onclick=\"setLanguage('zh-cn'); return false;\" style=\"color:rgba(255,255,255,0.9);text-decoration:none;padding:6px 12px;border:1px solid rgba(255,255,255,0.5);border-radius:4px;margin-left:8px;%s\">中文</a>\n"
        "  </span>\n"
        "</h1>\n"
        "<p>%s</p>\n"
        "<a href=\"/\" class=\"nav-link\">← %s</a>\n"
        "</div>\n"
        "<h2>💬 %s</h2>\n"
        "<ul class=\"thread-list\">\n",
        escaped_name_title ? escaped_name_title : "Board",
        (long long)board_id,
        escaped_name_h1 ? escaped_name_h1 : "board",
        escaped_name_body ? escaped_name_body : "Board",
        (lang == LANG_EN ? "background:rgba(255,255,255,0.2);" : ""),
        (lang == LANG_ZH_CN ? "background:rgba(255,255,255,0.2);" : ""),
        escaped_desc ? escaped_desc : "No description",
        i18n_get(lang, "back_to_boards"),
        i18n_get(lang, "threads"));
    
    free(escaped_name_title);
    free(escaped_name_h1);
    free(escaped_name_body);
    free(escaped_desc);
    
    sqlite3_stmt *stmt = db_prepare(
        "SELECT t.id, t.subject, COUNT(p.id) as post_count "
        "FROM threads t LEFT JOIN posts p ON t.id = p.thread_id "
        "WHERE t.board_id = ? "
        "GROUP BY t.id "
        "ORDER BY t.created_at DESC"
    );
    
    if (stmt) {
        sqlite3_bind_int64(stmt, 1, board_id);
        
        while (db_step(stmt) == SQLITE_ROW) {
            int64_t thread_id = sqlite3_column_int64(stmt, 0);
            const char *subject = (const char *)sqlite3_column_text(stmt, 1);
            int post_count = sqlite3_column_int(stmt, 2);
            
            char *escaped_subject = render_escape_html(subject ? subject : "No Subject");
            
            len += snprintf(html + len, 32768 - len,
                "<li class=\"thread-item\">\n"
                "<a href=\"/thread?id=%lld\" class=\"thread-link\">%s</a>\n"
                "<span class=\"thread-meta\">💬 %d posts</span>\n"
                "</li>\n",
                (long long)thread_id,
                escaped_subject ? escaped_subject : "No Subject",
                post_count);
            
            free(escaped_subject);
        }
        db_finalize(stmt);
    }
    
    len += snprintf(html + len, 65536 - len,
        "</ul>\n"
        "<div class=\"card\" style=\"margin-top:24px;\">\n"
        "<h2>✏️ %s</h2>\n"
        "<form method=\"POST\" action=\"/thread\">\n"
        "<input type=\"hidden\" name=\"board_id\" value=\"%lld\">\n"
        "<div class=\"form-group\">\n"
        "<label>%s</label>\n"
        "<input type=\"text\" name=\"subject\" required>\n"
        "</div>\n"
        "<div class=\"form-group\">\n"
        "<label>%s</label>\n"
        "<input type=\"text\" name=\"author\" placeholder=\"%s\">\n"
        "</div>\n"
        "<div class=\"form-group\">\n"
        "<label>%s</label>\n"
        "<textarea name=\"content\" required></textarea>\n"
        "</div>\n"
        "<div style=\"margin-bottom:16px;\">\n"
        "<button type=\"button\" class=\"kaomoji-btn\" onclick=\"openKaomoji()\">😊 %s</button>\n"
        "</div>\n"
        "<button type=\"submit\" class=\"btn\">%s</button>\n"
        "</form>\n"
        "</div>\n"
        "<div id=\"kaomoji-modal\" class=\"kaomoji-modal\">\n"
        "<div class=\"kaomoji-popup\">\n"
        "<div class=\"kaomoji-header\">\n"
        "<span class=\"kaomoji-title\">😊 %s</span>\n"
        "<button class=\"kaomoji-close\" onclick=\"closeKaomoji()\">×</button>\n"
        "</div>\n"
        "<div class=\"kaomoji-tabs\">\n",
        i18n_get(lang, "create_new_thread"),
        (long long)board_id,
        i18n_get(lang, "subject"),
        i18n_get(lang, "name"),
        i18n_get(lang, "anonymous"),
        i18n_get(lang, "content"),
        i18n_get(lang, "kaomoji"),
        i18n_get(lang, "create_thread"),
        i18n_get(lang, "kaomoji"));
    
    const kaomoji_category_t *categories = kaomoji_get_categories();
    int categories_count = kaomoji_get_categories_count();
    
    for (int i = 0; i < categories_count && len < 65536 - 1024; i++) {
        char *escaped_title = render_escape_html(categories[i].title);
        len += snprintf(html + len, 65536 - len,
            "<button class=\"kaomoji-tab%s\" onclick=\"switchTab(%d)\">%s</button>\n",
            (i == 0 ? " active" : ""),
            i,
            escaped_title ? escaped_title : categories[i].title);
        free(escaped_title);
    }
    
    len += snprintf(html + len, 65536 - len, "</div>\n<div class=\"kaomoji-content\">\n");
    
    for (int i = 0; i < categories_count && len < 65536 - 1024; i++) {
        len += snprintf(html + len, 65536 - len,
            "<div class=\"kaomoji-category%s\">\n"
            "<div class=\"kaomoji-items\">\n",
            (i == 0 ? " active" : ""));
        
        for (int j = 0; j < categories[i].count && len < 65536 - 512; j++) {
            char *escaped_js = render_escape_js(categories[i].items[j]);
            char *escaped_html = render_escape_html(categories[i].items[j]);
            len += snprintf(html + len, 65536 - len,
                "<span class=\"kaomoji-item\" onclick=\"insertKaomoji('%s')\">%s</span>\n",
                escaped_js ? escaped_js : categories[i].items[j],
                escaped_html ? escaped_html : categories[i].items[j]);
            free(escaped_js);
            free(escaped_html);
        }
        
        len += snprintf(html + len, 65536 - len,
            "</div>\n"
            "</div>\n");
    }
    
    len += snprintf(html + len, 65536 - len,
        "</div>\n"
        "</div>\n"
        "</div>\n"
        "</div>\n"
        "</body>\n"
        "</html>");
    
    board_free(board);
    http_response_t *response = http_response_create(200, "text/html", html, len);
    free(html);
    return response;
}

http_response_t *thread_view_handler(http_request_t *req) {
    language_t lang = i18n_get_language(req);
    
    int64_t thread_id = 1;
    if (req->query_string) {
        sscanf(req->query_string, "id=%lld", (long long *)&thread_id);
    }
    
    thread_t *thread = thread_get_by_id(thread_id);
    if (!thread) {
        char error_html[512];
        snprintf(error_html, sizeof(error_html),
            "<html><body><h1>%s</h1><a href=\"/\">%s</a></body></html>",
            i18n_get(lang, "thread_not_found"),
            i18n_get(lang, "back_to_boards"));
        return http_response_create(404, "text/html", error_html, strlen(error_html));
    }
    
    char *html = malloc(65536);
    if (!html) {
        thread_free(thread);
        const char *err = "<html><body><h1>Error: Out of memory</h1></body></html>";
        return http_response_create(500, "text/html", err, strlen(err));
    }
    
    char *escaped_subject_title = render_escape_html(thread->subject ? thread->subject : "Thread");
    char *escaped_subject_h1 = render_escape_html(thread->subject ? thread->subject : "Thread");
    char *escaped_author = render_escape_html(thread->author ? thread->author : "Anonymous");
    char *escaped_content = render_escape_html(thread->content ? thread->content : "No content");
    
    int len = snprintf(html, 65536,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<meta charset=\"UTF-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "<title>%s</title>\n"
        "<style>\n"
        ":root {\n"
        "  --primary: #1976d2; --primary-dark: #1565c0; --primary-light: #42a5f5;\n"
        "  --accent: #ff4081; --success: #4caf50; --text-primary: rgba(0,0,0,0.87);\n"
        "  --text-secondary: rgba(0,0,0,0.54); --divider: rgba(0,0,0,0.12);\n"
        "  --background: #fafafa; --surface: #ffffff; --error: #f44336;\n"
        "}\n"
        "* { box-sizing: border-box; margin: 0; padding: 0; }\n"
        "body { font-family: 'Roboto', 'Segoe UI', Arial, sans-serif, 'Microsoft YaHei', 'SimHei'; background: var(--background);\n"
        "  color: var(--text-primary); line-height: 1.6; padding: 16px; }\n"
        ".container { max-width: 1200px; margin: 0 auto; }\n"
        ".card { background: var(--surface); border-radius: 4px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);\n"
        "  padding: 16px; margin-bottom: 16px; transition: box-shadow 0.3s; }\n"
        ".card:hover { box-shadow: 0 4px 8px rgba(0,0,0,0.15); }\n"
        ".header-card { background: linear-gradient(135deg, var(--primary) 0%%, var(--primary-dark) 100%%);\n"
        "  color: white; padding: 24px; margin-bottom: 24px; }\n"
        ".header-card h1 { color: white; font-size: 2rem; font-weight: 500; margin-bottom: 8px; }\n"
        "@media (max-width: 768px) { .header-card h1 { font-size: 1.5rem; } }\n"
        ".nav-link { color: rgba(255,255,255,0.9); text-decoration: none; margin-right: 16px;\n"
        "  display: inline-block; margin-top: 12px; font-size: 0.875rem; transition: color 0.2s; }\n"
        ".nav-link:hover { color: white; text-decoration: underline; }\n"
        "h2 { font-size: 1.5rem; font-weight: 500; margin-bottom: 16px; color: var(--text-primary); }\n"
        ".op-post { background: linear-gradient(to right, #e3f2fd 0%%, var(--surface) 100%%);\n"
        "  border-left: 4px solid var(--primary); padding: 20px; margin-bottom: 24px;\n"
        "  border-radius: 4px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n"
        ".op-post .author { font-weight: 600; color: var(--primary); font-size: 1.125rem; }\n"
        ".op-post .content { margin-top: 12px; white-space: pre-wrap; word-wrap: break-word; }\n"
        ".post { background: var(--surface); border-radius: 4px; padding: 16px; margin-bottom: 12px;\n"
        "  box-shadow: 0 1px 3px rgba(0,0,0,0.1); transition: all 0.2s; position: relative; }\n"
        ".post:hover { box-shadow: 0 4px 8px rgba(0,0,0,0.15); }\n"
        ".post-header { display: flex; justify-content: space-between; align-items: center;\n"
        "  margin-bottom: 12px; flex-wrap: wrap; gap: 8px; }\n"
        ".post-info { display: flex; align-items: center; gap: 12px; flex: 1; }\n"
        ".post-author { font-weight: 500; color: var(--text-primary); }\n"
        ".post-id { color: var(--text-secondary); font-size: 0.875rem; }\n"
        ".reply-btn { background: var(--success); color: white; border: none; padding: 6px 16px;\n"
        "  cursor: pointer; border-radius: 4px; font-size: 0.875rem; font-weight: 500;\n"
        "  transition: all 0.2s; box-shadow: 0 1px 3px rgba(0,0,0,0.2); }\n"
        ".reply-btn:hover { background: #45a049; box-shadow: 0 2px 4px rgba(0,0,0,0.3); }\n"
        "@media (max-width: 768px) { .reply-btn { width: 100%%; margin-top: 8px; } }\n"
        ".quote-ref { color: var(--primary); cursor: pointer; text-decoration: none;\n"
        "  font-weight: 500; transition: color 0.2s; }\n"
        ".quote-ref:hover { color: var(--primary-dark); text-decoration: underline; }\n"
        ".quoted-post { display: none; background: #f5f5f5; border-left: 3px solid var(--primary);\n"
        "  padding: 12px; margin: 12px 0; font-size: 0.9em; border-radius: 2px; }\n"
        ".quoted-post.expanded { display: block; }\n"
        ".post-content { white-space: pre-wrap; word-wrap: break-word; line-height: 1.6; }\n"
        ".btn { background: var(--primary); color: white; border: none; padding: 10px 24px;\n"
        "  border-radius: 4px; font-size: 0.875rem; font-weight: 500; text-transform: uppercase;\n"
        "  cursor: pointer; box-shadow: 0 2px 4px rgba(0,0,0,0.2); transition: all 0.2s; min-height: 48px; }\n"
        ".btn:hover { background: var(--primary-dark); box-shadow: 0 4px 8px rgba(0,0,0,0.3); }\n"
        ".btn:active { box-shadow: 0 1px 2px rgba(0,0,0,0.2); }\n"
        "@media (max-width: 768px) { .btn { width: 100%%; } }\n"
        "input[type=\"text\"], textarea { width: 100%%; padding: 12px 16px; margin: 8px 0;\n"
        "  border: 1px solid var(--divider); border-radius: 4px; font-size: 1rem; font-family: inherit;\n"
        "  background: var(--surface); transition: border-color 0.2s; }\n"
        "input:focus, textarea:focus { outline: none; border-color: var(--primary);\n"
        "  box-shadow: 0 0 0 2px rgba(25,118,210,0.1); }\n"
        "textarea { min-height: 120px; resize: vertical; }\n"
        "label { display: block; margin-bottom: 4px; font-weight: 500; color: var(--text-primary); }\n"
        ".form-group { margin-bottom: 16px; }\n"
        ".kaomoji-btn { background: var(--accent); color: white; border: none; padding: 8px 16px;\n"
        "  cursor: pointer; border-radius: 4px; font-weight: 500; transition: background 0.2s;\n"
        "  font-size: 0.875rem; box-shadow: 0 2px 4px rgba(0,0,0,0.2); }\n"
        ".kaomoji-btn:hover { background: #e91e63; box-shadow: 0 3px 6px rgba(0,0,0,0.3); }\n"
        ".kaomoji-modal { display: none; position: fixed; z-index: 1000; left: 0; top: 0;\n"
        "  width: 100%%; height: 100%%; background: rgba(0,0,0,0.5); align-items: center;\n"
        "  justify-content: center; }\n"
        ".kaomoji-modal.show { display: flex; }\n"
        ".kaomoji-popup { background: var(--surface); border-radius: 8px; box-shadow: 0 4px 20px rgba(0,0,0,0.3);\n"
        "  max-width: 600px; width: 90%%; max-height: 70vh; display: flex; flex-direction: column;\n"
        "  position: relative; }\n"
        ".kaomoji-header { display: flex; justify-content: space-between; align-items: center;\n"
        "  padding: 16px 20px; border-bottom: 1px solid var(--divider); }\n"
        ".kaomoji-title { font-size: 1.125rem; font-weight: 500; color: var(--text-primary); }\n"
        ".kaomoji-close { background: none; border: none; font-size: 1.5rem; color: var(--text-secondary);\n"
        "  cursor: pointer; padding: 0; width: 32px; height: 32px; border-radius: 50%%;\n"
        "  transition: all 0.2s; line-height: 1; }\n"
        ".kaomoji-close:hover { background: rgba(0,0,0,0.05); color: var(--text-primary); }\n"
        ".kaomoji-tabs { display: flex; overflow-x: auto; border-bottom: 1px solid var(--divider);\n"
        "  background: #f5f5f5; }\n"
        ".kaomoji-tab { background: none; border: none; padding: 12px 20px; cursor: pointer;\n"
        "  font-size: 0.875rem; font-weight: 500; color: var(--text-secondary);\n"
        "  transition: all 0.2s; white-space: nowrap; border-bottom: 2px solid transparent; }\n"
        ".kaomoji-tab:hover { background: rgba(25,118,210,0.05); color: var(--primary); }\n"
        ".kaomoji-tab.active { color: var(--primary); border-bottom-color: var(--primary); background: white; }\n"
        ".kaomoji-content { flex: 1; overflow-y: auto; padding: 16px 20px; }\n"
        ".kaomoji-category { display: none; }\n"
        ".kaomoji-category.active { display: block; }\n"
        ".kaomoji-items { display: flex; flex-wrap: wrap; gap: 8px; }\n"
        ".kaomoji-item { padding: 8px 16px; border: 1px solid var(--divider); background: white;\n"
        "  cursor: pointer; border-radius: 4px; font-size: 1rem; transition: all 0.2s;\n"
        "  box-shadow: 0 1px 2px rgba(0,0,0,0.05); }\n"
        ".kaomoji-item:hover { background: #e3f2fd; border-color: var(--primary);\n"
        "  box-shadow: 0 2px 4px rgba(0,0,0,0.15); transform: translateY(-1px); }\n"
        ".kaomoji-item:active { transform: translateY(0); box-shadow: 0 1px 2px rgba(0,0,0,0.1); }\n"
        "@media (max-width: 768px) {\n"
        "  .kaomoji-popup { width: 95%%; max-height: 80vh; }\n"
        "  .kaomoji-tab { padding: 10px 12px; font-size: 0.8rem; }\n"
        "  .kaomoji-item { padding: 6px 12px; font-size: 0.9rem; }\n"
        "}\n"
        "</style>\n"
        "<script>\n"
        "var currentTab = 0;\n"
        "function replyToPost(postId) {\n"
        "  document.getElementById('reply_to').value = postId;\n"
        "  document.getElementById('reply-form').scrollIntoView({behavior:'smooth'});\n"
        "  document.getElementById('content').focus();\n"
        "}\n"
        "function toggleQuote(postId) {\n"
        "  var quote = document.getElementById('quote-' + postId);\n"
        "  if (quote) {\n"
        "    quote.classList.toggle('expanded');\n"
        "  }\n"
        "}\n"
        "function openKaomoji() {\n"
        "  document.getElementById('kaomoji-modal').classList.add('show');\n"
        "}\n"
        "function closeKaomoji() {\n"
        "  document.getElementById('kaomoji-modal').classList.remove('show');\n"
        "}\n"
        "function switchTab(index) {\n"
        "  currentTab = index;\n"
        "  var tabs = document.querySelectorAll('.kaomoji-tab');\n"
        "  var categories = document.querySelectorAll('.kaomoji-category');\n"
        "  tabs.forEach(function(tab, i) {\n"
        "    if (i === index) { tab.classList.add('active'); } else { tab.classList.remove('active'); }\n"
        "  });\n"
        "  categories.forEach(function(cat, i) {\n"
        "    if (i === index) { cat.classList.add('active'); } else { cat.classList.remove('active'); }\n"
        "  });\n"
        "}\n"
        "function insertKaomoji(kaomoji) {\n"
        "  var textarea = document.getElementById('content');\n"
        "  var start = textarea.selectionStart;\n"
        "  var end = textarea.selectionEnd;\n"
        "  var text = textarea.value;\n"
        "  textarea.value = text.substring(0, start) + kaomoji + text.substring(end);\n"
        "  textarea.selectionStart = textarea.selectionEnd = start + kaomoji.length;\n"
        "  textarea.focus();\n"
        "  closeKaomoji();\n"
        "}\n"
        "window.onclick = function(event) {\n"
        "  var modal = document.getElementById('kaomoji-modal');\n"
        "  if (event.target === modal) { closeKaomoji(); }\n"
        "}\n"
        "function setLanguage(lang) {\n"
        "  document.cookie = 'lang=' + lang + '; path=/; max-age=31536000';\n"
        "  window.location.href = window.location.pathname + '?id=%lld&lang=' + lang;\n"
        "}\n"
        "</script>\n"
        "</head>\n"
        "<body>\n"
        "<div class=\"container\">\n"
        "<div class=\"header-card card\">\n"
        "<h1 style=\"display:flex;justify-content:space-between;align-items:center;flex-wrap:wrap;\">\n"
        "  <span>%s</span>\n"
        "  <span style=\"font-size:0.875rem;font-weight:normal;\">\n"
        "    <a href=\"#\" onclick=\"setLanguage('en'); return false;\" style=\"color:rgba(255,255,255,0.9);text-decoration:none;padding:6px 12px;border:1px solid rgba(255,255,255,0.5);border-radius:4px;margin-left:8px;%s\">English</a>\n"
        "    <a href=\"#\" onclick=\"setLanguage('zh-cn'); return false;\" style=\"color:rgba(255,255,255,0.9);text-decoration:none;padding:6px 12px;border:1px solid rgba(255,255,255,0.5);border-radius:4px;margin-left:8px;%s\">中文</a>\n"
        "  </span>\n"
        "</h1>\n"
        "<a href=\"/board?id=%lld\" class=\"nav-link\">← %s</a>\n"
        "<a href=\"/\" class=\"nav-link\">🏠 %s</a>\n"
        "</div>\n"
        "<div class=\"op-post\">\n"
        "<div class=\"author\">👤 %s</div>\n"
        "<div class=\"content\">%s</div>\n"
        "</div>\n"
        "<h2>💬 %s</h2>\n",
        escaped_subject_title ? escaped_subject_title : "Thread",
        (long long)thread_id,
        escaped_subject_h1 ? escaped_subject_h1 : "Thread",
        (lang == LANG_EN ? "background:rgba(255,255,255,0.2);" : ""),
        (lang == LANG_ZH_CN ? "background:rgba(255,255,255,0.2);" : ""),
        (long long)thread->board_id,
        i18n_get(lang, "back_to_board"),
        i18n_get(lang, "all_boards"),
        escaped_author ? escaped_author : i18n_get(lang, "anonymous"),
        escaped_content ? escaped_content : "No content",
        i18n_get(lang, "posts"));
    
    free(escaped_subject_title);
    free(escaped_subject_h1);
    free(escaped_author);
    free(escaped_content);
    
    sqlite3_stmt *stmt = db_prepare(
        "SELECT p.id, p.author, p.content, p.created_at, p.reply_to, "
        "rp.id, rp.author, rp.content "
        "FROM posts p "
        "LEFT JOIN posts rp ON p.reply_to = rp.id "
        "WHERE p.thread_id = ? ORDER BY p.created_at ASC"
    );
    
    if (stmt) {
        sqlite3_bind_int64(stmt, 1, thread_id);
        
        while (db_step(stmt) == SQLITE_ROW) {
            int64_t post_id = sqlite3_column_int64(stmt, 0);
            const char *author = (const char *)sqlite3_column_text(stmt, 1);
            const char *content = (const char *)sqlite3_column_text(stmt, 2);
            int64_t reply_to = sqlite3_column_int64(stmt, 4);
            int64_t reply_to_id = sqlite3_column_int64(stmt, 5);
            const char *reply_to_author = (const char *)sqlite3_column_text(stmt, 6);
            const char *reply_to_content = (const char *)sqlite3_column_text(stmt, 7);
            
            char *escaped_author = render_escape_html(author ? author : "Anonymous");
            char *escaped_content = render_escape_html(content ? content : "");
            
            len += snprintf(html + len, 32768 - len,
                "<div class=\"post\" id=\"post-%lld\">\n"
                "<div class=\"post-header\">\n"
                "<div class=\"post-info\">\n"
                "<span class=\"post-author\">%s</span>\n"
                "<span class=\"post-id\">#%lld</span>",
                (long long)post_id,
                escaped_author ? escaped_author : "Anonymous",
                (long long)post_id);
            
            if (reply_to > 0 && reply_to_id > 0) {
                len += snprintf(html + len, 32768 - len,
                    "<span class=\"quote-ref\" onclick=\"toggleQuote(%lld)\">&gt;&gt;%lld</span>",
                    (long long)reply_to_id,
                    (long long)reply_to_id);
            }
            
            len += snprintf(html + len, 65536 - len,
                "</div>\n"
                "<button class=\"reply-btn\" onclick=\"replyToPost(%lld)\">↩ %s</button>\n"
                "</div>\n",
                (long long)post_id,
                i18n_get(lang, "reply"));
            
            if (reply_to > 0 && reply_to_id > 0 && reply_to_content) {
                char *escaped_reply_author = render_escape_html(reply_to_author ? reply_to_author : "Anonymous");
                char *escaped_reply_content = render_escape_html(reply_to_content);
                
                len += snprintf(html + len, 32768 - len,
                    "<div class=\"quoted-post\" id=\"quote-%lld\">\n"
                    "<strong>%s</strong> (#%lld): %s\n"
                    "</div>\n",
                    (long long)reply_to_id,
                    escaped_reply_author ? escaped_reply_author : "Anonymous",
                    (long long)reply_to_id,
                    escaped_reply_content ? escaped_reply_content : "");
                
                free(escaped_reply_author);
                free(escaped_reply_content);
            }
            
            len += snprintf(html + len, 32768 - len,
                "<div class=\"post-content\">%s</div>\n"
                "</div>\n",
                escaped_content ? escaped_content : "");
            
            free(escaped_author);
            free(escaped_content);
        }
        db_finalize(stmt);
    }
    
    len += snprintf(html + len, 65536 - len,
        "<div class=\"card\" style=\"margin-top:24px;\">\n"
        "<h2>✏️ %s</h2>\n"
        "<form id=\"reply-form\" method=\"POST\" action=\"/post\">\n"
        "<input type=\"hidden\" name=\"thread_id\" value=\"%lld\">\n"
        "<input type=\"hidden\" id=\"reply_to\" name=\"reply_to\" value=\"\">\n"
        "<div class=\"form-group\">\n"
        "<label>%s</label>\n"
        "<input type=\"text\" name=\"author\" placeholder=\"%s\">\n"
        "</div>\n"
        "<div class=\"form-group\">\n"
        "<label>%s</label>\n"
        "<textarea id=\"content\" name=\"content\" required></textarea>\n"
        "</div>\n"
        "<div style=\"margin-bottom:16px;\">\n"
        "<button type=\"button\" class=\"kaomoji-btn\" onclick=\"openKaomoji()\">😊 %s</button>\n"
        "</div>\n"
        "<button type=\"submit\" class=\"btn\">%s</button>\n"
        "</form>\n"
        "</div>\n"
        "<div id=\"kaomoji-modal\" class=\"kaomoji-modal\">\n"
        "<div class=\"kaomoji-popup\">\n"
        "<div class=\"kaomoji-header\">\n"
        "<span class=\"kaomoji-title\">😊 %s</span>\n"
        "<button class=\"kaomoji-close\" onclick=\"closeKaomoji()\">×</button>\n"
        "</div>\n"
        "<div class=\"kaomoji-tabs\">\n",
        i18n_get(lang, "reply"),
        (long long)thread_id,
        i18n_get(lang, "name"),
        i18n_get(lang, "anonymous"),
        i18n_get(lang, "content"),
        i18n_get(lang, "kaomoji"),
        i18n_get(lang, "post_reply"),
        i18n_get(lang, "kaomoji"));
    
    const kaomoji_category_t *categories2 = kaomoji_get_categories();
    int categories_count2 = kaomoji_get_categories_count();
    
    for (int i = 0; i < categories_count2 && len < 65536 - 1024; i++) {
        char *escaped_title = render_escape_html(categories2[i].title);
        len += snprintf(html + len, 65536 - len,
            "<button class=\"kaomoji-tab%s\" onclick=\"switchTab(%d)\">%s</button>\n",
            (i == 0 ? " active" : ""),
            i,
            escaped_title ? escaped_title : categories2[i].title);
        free(escaped_title);
    }
    
    len += snprintf(html + len, 65536 - len, "</div>\n<div class=\"kaomoji-content\">\n");
    
    for (int i = 0; i < categories_count2 && len < 65536 - 1024; i++) {
        len += snprintf(html + len, 65536 - len,
            "<div class=\"kaomoji-category%s\">\n"
            "<div class=\"kaomoji-items\">\n",
            (i == 0 ? " active" : ""));
        
        for (int j = 0; j < categories2[i].count && len < 65536 - 512; j++) {
            char *escaped_js = render_escape_js(categories2[i].items[j]);
            char *escaped_html = render_escape_html(categories2[i].items[j]);
            len += snprintf(html + len, 65536 - len,
                "<span class=\"kaomoji-item\" onclick=\"insertKaomoji('%s')\">%s</span>\n",
                escaped_js ? escaped_js : categories2[i].items[j],
                escaped_html ? escaped_html : categories2[i].items[j]);
            free(escaped_js);
            free(escaped_html);
        }
        
        len += snprintf(html + len, 65536 - len,
            "</div>\n"
            "</div>\n");
    }
    
    len += snprintf(html + len, 65536 - len,
        "</div>\n"
        "</div>\n"
        "</div>\n"
        "</div>\n"
        "</body>\n"
        "</html>");
    
    thread_free(thread);
    http_response_t *response = http_response_create(200, "text/html", html, len);
    free(html);
    return response;
}

http_response_t *thread_create_handler(http_request_t *req) {
    language_t lang = i18n_get_language(req);
    
    if (!req->body) {
        char error_html[256];
        snprintf(error_html, sizeof(error_html),
            "<html><body><h1>%s: %s</h1></body></html>",
            i18n_get(lang, "error"),
            i18n_get(lang, "no_form_data"));
        return http_response_create(400, "text/html", error_html, strlen(error_html));
    }
    
    int64_t board_id = 1;
    char subject[256] = {0};
    char author[128] = "Anonymous";
    char content[2048] = {0};
    char decoded_value[2048];
    
    char *body_copy = strdup(req->body);
    if (!body_copy) {
        char error_html[256];
        snprintf(error_html, sizeof(error_html),
            "<html><body><h1>%s: %s</h1></body></html>",
            i18n_get(lang, "error"),
            i18n_get(lang, "out_of_memory"));
        return http_response_create(500, "text/html", error_html, strlen(error_html));
    }
    
    char *token = strtok(body_copy, "&");
    while (token) {
        char *eq = strchr(token, '=');
        if (eq) {
            *eq = '\0';
            char *key = token;
            char *value = eq + 1;
            
            if (strcmp(key, "board_id") == 0) {
                board_id = atoll(value);
            } else if (strcmp(key, "subject") == 0) {
                url_decode(subject, value, sizeof(subject));
            } else if (strcmp(key, "author") == 0 && strlen(value) > 0) {
                url_decode(decoded_value, value, sizeof(decoded_value));
                if (strlen(decoded_value) > 0) {
                    strncpy(author, decoded_value, sizeof(author) - 1);
                }
            } else if (strcmp(key, "content") == 0) {
                url_decode(content, value, sizeof(content));
            }
        }
        token = strtok(NULL, "&");
    }
    free(body_copy);
    
    sqlite3_stmt *stmt = db_prepare(
        "INSERT INTO threads (board_id, subject) VALUES (?, ?)"
    );
    if (!stmt) {
        char error_html[256];
        snprintf(error_html, sizeof(error_html),
            "<html><body><h1>%s: Failed to create thread</h1></body></html>",
            i18n_get(lang, "error"));
        return http_response_create(500, "text/html", error_html, strlen(error_html));
    }
    
    sqlite3_bind_int64(stmt, 1, board_id);
    sqlite3_bind_text(stmt, 2, subject, -1, SQLITE_TRANSIENT);
    
    int rc = db_step(stmt);
    int64_t thread_id = sqlite3_last_insert_rowid(db_get_connection());
    db_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        char error_html[256];
        snprintf(error_html, sizeof(error_html),
            "<html><body><h1>%s: Failed to create thread</h1></body></html>",
            i18n_get(lang, "error"));
        return http_response_create(500, "text/html", error_html, strlen(error_html));
    }
    
    sqlite3_stmt *post_stmt = db_prepare(
        "INSERT INTO posts (thread_id, author, content, reply_to) VALUES (?, ?, ?, ?)"
    );
    if (post_stmt) {
        sqlite3_bind_int64(post_stmt, 1, thread_id);
        sqlite3_bind_text(post_stmt, 2, author, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(post_stmt, 3, content, -1, SQLITE_TRANSIENT);
        sqlite3_bind_null(post_stmt, 4);
        db_step(post_stmt);
        db_finalize(post_stmt);
    }
    
    char *html = malloc(1024);
    if (!html) {
        char error_html[256];
        snprintf(error_html, sizeof(error_html),
            "<html><body><h1>%s: %s</h1></body></html>",
            i18n_get(lang, "error"),
            i18n_get(lang, "out_of_memory"));
        return http_response_create(500, "text/html", error_html, strlen(error_html));
    }
    
    int len = snprintf(html, 1024,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<meta charset=\"UTF-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "<title>%s</title>\n"
        "<style>\n"
        "body{font-family:'Roboto','Segoe UI',Arial,sans-serif,'Microsoft YaHei','SimHei';background:#fafafa;margin:0;padding:20px;text-align:center;}\n"
        ".container{max-width:600px;margin:50px auto;background:#fff;padding:32px;border-radius:8px;box-shadow:0 4px 8px rgba(0,0,0,0.1);}\n"
        "h1{color:#4caf50;margin-bottom:16px;font-size:2rem;}\n"
        "p{color:rgba(0,0,0,0.87);margin:20px 0;font-size:1.1rem;}\n"
        ".btn{background:#1976d2;color:#fff;padding:12px 24px;border-radius:4px;text-decoration:none;display:inline-block;margin:8px;transition:background 0.2s;}\n"
        ".btn:hover{background:#1565c0;}\n"
        "</style>\n"
        "</head>\n"
        "<body>\n"
        "<div class=\"container\">\n"
        "<h1>✅ %s</h1>\n"
        "<p>%s</p>\n"
        "<a href=\"/thread?id=%lld\" class=\"btn\">%s</a>\n"
        "<a href=\"/board?id=%lld\" class=\"btn\">%s</a>\n"
        "</div>\n"
        "</body>\n"
        "</html>",
        i18n_get(lang, "thread_created"),
        i18n_get(lang, "thread_created"),
        i18n_get(lang, "thread_created_msg"),
        (long long)thread_id,
        i18n_get(lang, "view_thread"),
        (long long)board_id,
        i18n_get(lang, "back_to_board"));
    
    http_response_t *response = http_response_create(200, "text/html", html, len);
    free(html);
    return response;
}

http_response_t *post_create_handler(http_request_t *req) {
    language_t lang = i18n_get_language(req);
    
    if (!req->body) {
        char error_html[256];
        snprintf(error_html, sizeof(error_html),
            "<html><body><h1>%s: %s</h1></body></html>",
            i18n_get(lang, "error"),
            i18n_get(lang, "no_form_data"));
        return http_response_create(400, "text/html", error_html, strlen(error_html));
    }
    
    int64_t thread_id = 0;
    int64_t reply_to = 0;
    char author[128] = "Anonymous";
    char content[2048] = {0};
    char decoded_value[2048];
    
    char *body_copy = strdup(req->body);
    if (!body_copy) {
        char error_html[256];
        snprintf(error_html, sizeof(error_html),
            "<html><body><h1>%s: %s</h1></body></html>",
            i18n_get(lang, "error"),
            i18n_get(lang, "out_of_memory"));
        return http_response_create(500, "text/html", error_html, strlen(error_html));
    }
    
    char *token = strtok(body_copy, "&");
    while (token) {
        char *eq = strchr(token, '=');
        if (eq) {
            *eq = '\0';
            char *key = token;
            char *value = eq + 1;
            
            if (strcmp(key, "thread_id") == 0) {
                thread_id = atoll(value);
            } else if (strcmp(key, "reply_to") == 0 && strlen(value) > 0) {
                reply_to = atoll(value);
            } else if (strcmp(key, "author") == 0 && strlen(value) > 0) {
                url_decode(decoded_value, value, sizeof(decoded_value));
                if (strlen(decoded_value) > 0) {
                    strncpy(author, decoded_value, sizeof(author) - 1);
                }
            } else if (strcmp(key, "content") == 0) {
                url_decode(content, value, sizeof(content));
            }
        }
        token = strtok(NULL, "&");
    }
    free(body_copy);
    
    if (thread_id == 0) {
        char error_html[256];
        snprintf(error_html, sizeof(error_html),
            "<html><body><h1>%s: Invalid thread ID</h1></body></html>",
            i18n_get(lang, "error"));
        return http_response_create(400, "text/html", error_html, strlen(error_html));
    }
    
    sqlite3_stmt *stmt = db_prepare(
        "INSERT INTO posts (thread_id, author, content, reply_to) VALUES (?, ?, ?, ?)"
    );
    if (!stmt) {
        char error_html[256];
        snprintf(error_html, sizeof(error_html),
            "<html><body><h1>%s: Failed to create post</h1></body></html>",
            i18n_get(lang, "error"));
        return http_response_create(500, "text/html", error_html, strlen(error_html));
    }
    
    sqlite3_bind_int64(stmt, 1, thread_id);
    sqlite3_bind_text(stmt, 2, author, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, content, -1, SQLITE_TRANSIENT);
    if (reply_to > 0) {
        sqlite3_bind_int64(stmt, 4, reply_to);
    } else {
        sqlite3_bind_null(stmt, 4);
    }
    
    int rc = db_step(stmt);
    db_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        char error_html[256];
        snprintf(error_html, sizeof(error_html),
            "<html><body><h1>%s: Failed to create post</h1></body></html>",
            i18n_get(lang, "error"));
        return http_response_create(500, "text/html", error_html, strlen(error_html));
    }
    
    char *html = malloc(1024);
    if (!html) {
        char error_html[256];
        snprintf(error_html, sizeof(error_html),
            "<html><body><h1>%s: %s</h1></body></html>",
            i18n_get(lang, "error"),
            i18n_get(lang, "out_of_memory"));
        return http_response_create(500, "text/html", error_html, strlen(error_html));
    }
    
    int len = snprintf(html, 1024,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<meta charset=\"UTF-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "<title>%s</title>\n"
        "<style>\n"
        "body{font-family:'Roboto','Segoe UI',Arial,sans-serif,'Microsoft YaHei','SimHei';background:#fafafa;margin:0;padding:20px;text-align:center;}\n"
        ".container{max-width:600px;margin:50px auto;background:#fff;padding:32px;border-radius:8px;box-shadow:0 4px 8px rgba(0,0,0,0.1);}\n"
        "h1{color:#4caf50;margin-bottom:16px;font-size:2rem;}\n"
        "p{color:rgba(0,0,0,0.87);margin:20px 0;font-size:1.1rem;}\n"
        ".btn{background:#1976d2;color:#fff;padding:12px 24px;border-radius:4px;text-decoration:none;display:inline-block;margin-top:16px;transition:background 0.2s;}\n"
        ".btn:hover{background:#1565c0;}\n"
        "</style>\n"
        "</head>\n"
        "<body>\n"
        "<div class=\"container\">\n"
        "<h1>✅ %s</h1>\n"
        "<p>%s</p>\n"
        "<a href=\"/thread?id=%lld\" class=\"btn\">%s</a>\n"
        "</div>\n"
        "</body>\n"
        "</html>",
        i18n_get(lang, "post_created"),
        i18n_get(lang, "post_created"),
        i18n_get(lang, "post_created_msg"),
        (long long)thread_id,
        i18n_get(lang, "back_to_thread"));
    
    http_response_t *response = http_response_create(200, "text/html", html, len);
    free(html);
    return response;
}

board_t *board_get_by_id(int64_t id) {
    sqlite3_stmt *stmt = db_prepare("SELECT id, name, description FROM boards WHERE id = ?");
    if (!stmt) {
        return NULL;
    }
    
    sqlite3_bind_int64(stmt, 1, id);
    
    board_t *board = NULL;
    if (db_step(stmt) == SQLITE_ROW) {
        board = malloc(sizeof(board_t));
        if (board) {
            board->id = sqlite3_column_int64(stmt, 0);
            const char *name = (const char *)sqlite3_column_text(stmt, 1);
            const char *desc = (const char *)sqlite3_column_text(stmt, 2);
            board->name = name ? strdup(name) : NULL;
            board->description = desc ? strdup(desc) : NULL;
        }
    }
    
    db_finalize(stmt);
    return board;
}

thread_t *thread_get_by_id(int64_t id) {
    sqlite3_stmt *stmt = db_prepare(
        "SELECT t.id, t.board_id, t.subject, p.content, p.author, t.created_at "
        "FROM threads t LEFT JOIN posts p ON t.id = p.thread_id "
        "WHERE t.id = ? ORDER BY p.id ASC LIMIT 1"
    );
    if (!stmt) {
        return NULL;
    }
    
    sqlite3_bind_int64(stmt, 1, id);
    
    thread_t *thread = NULL;
    if (db_step(stmt) == SQLITE_ROW) {
        thread = malloc(sizeof(thread_t));
        if (thread) {
            thread->id = sqlite3_column_int64(stmt, 0);
            thread->board_id = sqlite3_column_int64(stmt, 1);
            const char *subject = (const char *)sqlite3_column_text(stmt, 2);
            const char *content = (const char *)sqlite3_column_text(stmt, 3);
            const char *author = (const char *)sqlite3_column_text(stmt, 4);
            thread->subject = subject ? strdup(subject) : NULL;
            thread->content = content ? strdup(content) : NULL;
            thread->author = author ? strdup(author) : strdup("Anonymous");
            thread->created_at = sqlite3_column_int64(stmt, 5);
        }
    }
    
    db_finalize(stmt);
    return thread;
}

void board_free(board_t *board) {
    if (board) {
        free(board->name);
        free(board->description);
        free(board);
    }
}

void thread_free(thread_t *thread) {
    if (thread) {
        free(thread->subject);
        free(thread->content);
        free(thread->author);
        free(thread);
    }
}

void post_free(post_t *post) {
    if (post) {
        free(post->content);
        free(post->author);
        free(post);
    }
}
