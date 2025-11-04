#include "admin.h"
#include "router.h"
#include "render.h"
#include "db.h"
#include "i18n.h"
#include "auth.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int admin_is_authenticated(http_request_t *req) {
    return auth_is_authenticated(req);
}

void admin_init(void) {
    printf("Admin module initialized\n");
}

void admin_register_routes(void) {
    router_add_route("GET", "/admin", admin_dashboard_handler);
    router_add_route("GET", "/admin/login", admin_login_handler);
    router_add_route("POST", "/admin/login", admin_login_handler);
    router_add_route("GET", "/admin/logout", admin_logout_handler);
    router_add_route("GET", "/admin/change-password", admin_change_password_handler);
    router_add_route("POST", "/admin/change-password", admin_change_password_handler);
}

http_response_t *admin_dashboard_handler(http_request_t *req) {
    if (!admin_is_authenticated(req)) {
        const char *html = 
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<meta charset=\"UTF-8\">\n"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
            "<title>Access Denied</title>\n"
            "<style>\n"
            "body{font-family:'Roboto','Segoe UI',Arial,sans-serif;background:#fafafa;margin:0;padding:20px;}\n"
            ".container{max-width:600px;margin:50px auto;background:#fff;padding:32px;border-radius:8px;box-shadow:0 4px 8px rgba(0,0,0,0.1);text-align:center;}\n"
            "h1{color:#d32f2f;margin-bottom:16px;}\n"
            ".btn{background:#1976d2;color:#fff;padding:12px 24px;border-radius:4px;text-decoration:none;display:inline-block;margin-top:16px;}\n"
            "</style>\n"
            "</head>\n"
            "<body>\n"
            "<div class=\"container\">\n"
            "<h1>🔒 Access Denied</h1>\n"
            "<p>You must be logged in to access this page.</p>\n"
            "<a href=\"/admin/login\" class=\"btn\">Login</a>\n"
            "</div>\n"
            "</body>\n"
            "</html>";
        return http_response_create(403, "text/html", html, strlen(html));
    }
    
    char *html = malloc(4096);
    if (!html) {
        const char *err = "<html><body><h1>Error: Out of memory</h1></body></html>";
        return http_response_create(500, "text/html", err, strlen(err));
    }
    
    int board_count = 0, thread_count = 0, post_count = 0;
    
    sqlite3_stmt *stmt = db_prepare("SELECT COUNT(*) FROM boards");
    if (stmt) {
        if (db_step(stmt) == SQLITE_ROW) {
            board_count = sqlite3_column_int(stmt, 0);
        }
        db_finalize(stmt);
    }
    
    stmt = db_prepare("SELECT COUNT(*) FROM threads");
    if (stmt) {
        if (db_step(stmt) == SQLITE_ROW) {
            thread_count = sqlite3_column_int(stmt, 0);
        }
        db_finalize(stmt);
    }
    
    stmt = db_prepare("SELECT COUNT(*) FROM posts");
    if (stmt) {
        if (db_step(stmt) == SQLITE_ROW) {
            post_count = sqlite3_column_int(stmt, 0);
        }
        db_finalize(stmt);
    }
    
    int len = snprintf(html, 4096,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<meta charset=\"UTF-8\">\n"
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "<title>Admin Dashboard</title>\n"
        "<style>\n"
        ":root{--primary:#1976d2;--surface:#fff;--bg:#fafafa;--text:rgba(0,0,0,0.87);--secondary:rgba(0,0,0,0.54);}\n"
        "*{box-sizing:border-box;margin:0;padding:0;}\n"
        "body{font-family:'Roboto','Segoe UI',Arial,sans-serif;background:var(--bg);color:var(--text);padding:16px;}\n"
        ".container{max-width:1200px;margin:0 auto;}\n"
        ".header{background:linear-gradient(135deg,var(--primary),#1565c0);color:#fff;padding:24px;border-radius:4px;margin-bottom:24px;box-shadow:0 2px 4px rgba(0,0,0,0.2);}\n"
        ".header h1{font-size:2rem;font-weight:500;margin-bottom:16px;}\n"
        ".nav{display:flex;gap:16px;flex-wrap:wrap;}\n"
        ".nav a{color:rgba(255,255,255,0.9);text-decoration:none;font-size:0.875rem;transition:color 0.2s;}\n"
        ".nav a:hover{color:#fff;text-decoration:underline;}\n"
        "@media(max-width:768px){.header h1{font-size:1.5rem;}.nav{flex-direction:column;gap:8px;}}\n"
        ".card{background:var(--surface);border-radius:4px;box-shadow:0 2px 4px rgba(0,0,0,0.1);padding:24px;margin-bottom:16px;}\n"
        "h2{font-size:1.5rem;font-weight:500;margin-bottom:16px;}\n"
        "h3{font-size:1.25rem;font-weight:500;margin:16px 0 12px;color:var(--primary);}\n"
        ".stats{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:16px;}\n"
        ".stat-card{background:var(--surface);padding:20px;border-radius:4px;border-left:4px solid var(--primary);box-shadow:0 1px 3px rgba(0,0,0,0.1);}\n"
        ".stat-value{font-size:2rem;font-weight:500;color:var(--primary);}\n"
        ".stat-label{color:var(--secondary);font-size:0.875rem;margin-top:4px;}\n"
        "ul{list-style:none;}\n"
        "li{padding:12px;margin-bottom:8px;background:var(--surface);border-radius:4px;box-shadow:0 1px 2px rgba(0,0,0,0.05);}\n"
        "li a{color:var(--primary);text-decoration:none;}\n"
        "li a:hover{text-decoration:underline;}\n"
        "</style>\n"
        "</head>\n"
        "<body>\n"
        "<div class=\"container\">\n"
        "<div class=\"header\">\n"
        "<h1>⚙️ Admin Dashboard</h1>\n"
        "<div class=\"nav\">\n"
        "<a href=\"/\">← Back to Site</a>\n"
        "<a href=\"/admin/change-password\">🔑 Change Password</a>\n"
        "<a href=\"/admin/logout\">🚪 Logout</a>\n"
        "</div>\n"
        "</div>\n"
        "<div class=\"card\">\n"
        "<h2>📊 Statistics</h2>\n"
        "<div class=\"stats\">\n"
        "<div class=\"stat-card\"><div class=\"stat-value\">%d</div><div class=\"stat-label\">Total Boards</div></div>\n"
        "<div class=\"stat-card\"><div class=\"stat-value\">%d</div><div class=\"stat-label\">Total Threads</div></div>\n"
        "<div class=\"stat-card\"><div class=\"stat-value\">%d</div><div class=\"stat-label\">Total Posts</div></div>\n"
        "</div>\n"
        "</div>\n"
        "<div class=\"card\">\n"
        "<h2>🔥 Recent Activity</h2>\n"
        "<h3>Latest Threads</h3>\n"
        "<ul>\n",
        board_count, thread_count, post_count);
    
    stmt = db_prepare(
        "SELECT t.id, t.subject, b.name "
        "FROM threads t JOIN boards b ON t.board_id = b.id "
        "ORDER BY t.created_at DESC LIMIT 10"
    );
    if (stmt) {
        while (db_step(stmt) == SQLITE_ROW) {
            int64_t id = sqlite3_column_int64(stmt, 0);
            const char *subject = (const char *)sqlite3_column_text(stmt, 1);
            const char *board = (const char *)sqlite3_column_text(stmt, 2);
            
            len += snprintf(html + len, 4096 - len,
                "<li><a href=\"/thread?id=%lld\">%s</a> in /%s/</li>\n",
                (long long)id,
                subject ? subject : "No Subject",
                board ? board : "unknown");
        }
        db_finalize(stmt);
    }
    
    len += snprintf(html + len, 4096 - len,
        "</ul>\n"
        "</div>\n"
        "</div>\n"
        "</body>\n"
        "</html>");
    
    http_response_t *response = http_response_create(200, "text/html", html, len);
    free(html);
    return response;
}

http_response_t *admin_login_handler(http_request_t *req) {
    language_t lang = i18n_get_language(req);
    
    if (strcmp(req->method, "GET") == 0) {
        if (admin_is_authenticated(req)) {
            char html[512];
            snprintf(html, sizeof(html),
                "<!DOCTYPE html>\n"
                "<html>\n"
                "<head>\n"
                "<meta http-equiv=\"refresh\" content=\"0; url=/admin\">\n"
                "</head>\n"
                "<body>\n"
                "<p>%s</p>\n"
                "</body>\n"
                "</html>",
                i18n_get(lang, "already_logged_in"));
            return http_response_create(200, "text/html", html, strlen(html));
        }
        
        char *html = malloc(4096);
        if (!html) {
            const char *err = "<html><body><h1>Error: Out of memory</h1></body></html>";
            return http_response_create(500, "text/html", err, strlen(err));
        }
        
        int len = snprintf(html, 4096,
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<meta charset=\"UTF-8\">\n"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
            "<title>%s</title>\n"
            "<style>\n"
            ":root{--primary:#1976d2;--primary-dark:#1565c0;}\n"
            "*{box-sizing:border-box;margin:0;padding:0;}\n"
            "body{font-family:'Roboto','Segoe UI',Arial,sans-serif,'Microsoft YaHei','SimHei';background:linear-gradient(135deg,#667eea 0%%,#764ba2 100%%);min-height:100vh;display:flex;align-items:center;justify-content:center;padding:16px;}\n"
            ".login-card{background:#fff;max-width:400px;width:100%%;padding:40px;border-radius:8px;box-shadow:0 8px 16px rgba(0,0,0,0.2);}\n"
            "h1{color:var(--primary);font-size:2rem;font-weight:500;margin-bottom:24px;text-align:center;}\n"
            ".form-group{margin-bottom:20px;}\n"
            "label{display:block;margin-bottom:8px;font-weight:500;color:rgba(0,0,0,0.87);}\n"
            "input[type=\"text\"],input[type=\"password\"]{width:100%%;padding:12px 16px;border:1px solid rgba(0,0,0,0.12);border-radius:4px;font-size:1rem;font-family:inherit;transition:border-color 0.2s;}\n"
            "input:focus{outline:none;border-color:var(--primary);box-shadow:0 0 0 2px rgba(25,118,210,0.1);}\n"
            ".btn{width:100%%;padding:12px;background:var(--primary);color:#fff;border:none;border-radius:4px;font-size:1rem;font-weight:500;cursor:pointer;box-shadow:0 2px 4px rgba(0,0,0,0.2);transition:all 0.2s;text-transform:uppercase;}\n"
            ".btn:hover{background:var(--primary-dark);box-shadow:0 4px 8px rgba(0,0,0,0.3);}\n"
            ".btn:active{box-shadow:0 1px 2px rgba(0,0,0,0.2);}\n"
            ".link{text-align:center;margin-top:20px;}\n"
            ".link a{color:var(--primary);text-decoration:none;}\n"
            ".link a:hover{text-decoration:underline;}\n"
            ".note{text-align:center;margin-top:16px;color:rgba(0,0,0,0.54);font-size:0.875rem;}\n"
            ".lang-switch{text-align:center;margin-bottom:16px;}\n"
            ".lang-switch a{color:var(--primary);text-decoration:none;padding:6px 12px;border:1px solid var(--primary);border-radius:4px;margin:0 4px;transition:all 0.2s;display:inline-block;}\n"
            ".lang-switch a:hover{background:var(--primary);color:#fff;}\n"
            ".lang-switch a.active{background:var(--primary);color:#fff;}\n"
            "@media(max-width:480px){.login-card{padding:24px;}h1{font-size:1.5rem;}}\n"
            "</style>\n"
            "<script>\n"
            "function setLanguage(lang) {\n"
            "  document.cookie = 'lang=' + lang + '; path=/; max-age=31536000';\n"
            "  window.location.href = '/admin/login?lang=' + lang;\n"
            "}\n"
            "</script>\n"
            "</head>\n"
            "<body>\n"
            "<div class=\"login-card\">\n"
            "<div class=\"lang-switch\">\n"
            "<a href=\"#\" onclick=\"setLanguage('en'); return false;\" class=\"%s\">English</a>\n"
            "<a href=\"#\" onclick=\"setLanguage('zh-cn'); return false;\" class=\"%s\">中文</a>\n"
            "</div>\n"
            "<h1>🔐 %s</h1>\n"
            "<form method=\"POST\" action=\"/admin/login\">\n"
            "<div class=\"form-group\">\n"
            "<label for=\"username\">%s</label>\n"
            "<input type=\"text\" id=\"username\" name=\"username\" required>\n"
            "</div>\n"
            "<div class=\"form-group\">\n"
            "<label for=\"password\">%s</label>\n"
            "<input type=\"password\" id=\"password\" name=\"password\" required>\n"
            "</div>\n"
            "<button type=\"submit\" class=\"btn\">%s</button>\n"
            "</form>\n"
            "<div class=\"link\"><a href=\"/\">← %s</a></div>\n"
            "<div class=\"note\">%s</div>\n"
            "</div>\n"
            "</body>\n"
            "</html>",
            i18n_get(lang, "admin_login"),
            (lang == LANG_EN ? "active" : ""),
            (lang == LANG_ZH_CN ? "active" : ""),
            i18n_get(lang, "admin_login"),
            i18n_get(lang, "username"),
            i18n_get(lang, "password"),
            i18n_get(lang, "login"),
            i18n_get(lang, "back_to_site"),
            i18n_get(lang, "default_credentials"));
        
        http_response_t *response = http_response_create(200, "text/html", html, len);
        free(html);
        return response;
    } else if (strcmp(req->method, "POST") == 0) {
        if (!req->body) {
            const char *html = "<html><body><h1>Bad Request</h1></body></html>";
            return http_response_create(400, "text/html", html, strlen(html));
        }
        
        char username[64] = {0};
        char password[64] = {0};
        
        char *user_param = strstr(req->body, "username=");
        char *pass_param = strstr(req->body, "password=");
        
        if (user_param) {
            user_param += 9;
            char *end = strchr(user_param, '&');
            size_t len = end ? (size_t)(end - user_param) : strlen(user_param);
            if (len < sizeof(username)) {
                memcpy(username, user_param, len);
                username[len] = '\0';
            }
        }
        
        if (pass_param) {
            pass_param += 9;
            char *end = strchr(pass_param, '&');
            size_t len = end ? (size_t)(end - pass_param) : strlen(pass_param);
            if (len < sizeof(password)) {
                memcpy(password, pass_param, len);
                password[len] = '\0';
            }
        }
        
        sqlite3_stmt *stmt = db_prepare(
            "SELECT id FROM admin_users WHERE username = ? AND password = ?"
        );
        
        if (!stmt) {
            const char *html = "<html><body><h1>Server Error</h1></body></html>";
            return http_response_create(500, "text/html", html, strlen(html));
        }
        
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
        
        int user_id = -1;
        if (db_step(stmt) == SQLITE_ROW) {
            user_id = sqlite3_column_int(stmt, 0);
        }
        db_finalize(stmt);
        
        if (user_id > 0) {
            char *token = auth_create_session(user_id);
            if (token) {
                char *cookie = malloc(256);
                if (cookie) {
                    snprintf(cookie, 256, "admin_session=%s; Path=/; Max-Age=604800; HttpOnly",
                             token);
                    
                    const char *html = 
                        "<!DOCTYPE html>\n"
                        "<html>\n"
                        "<head>\n"
                        "<meta http-equiv=\"refresh\" content=\"0; url=/admin\">\n"
                        "</head>\n"
                        "<body>\n"
                        "<p>Login successful! Redirecting...</p>\n"
                        "</body>\n"
                        "</html>";
                    
                    http_response_t *response = http_response_create(200, "text/html",
                                                                      html, strlen(html));
                    if (response) {
                        response->set_cookie = cookie;
                    }
                    return response;
                }
            }
        }
        
        const char *html = 
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<title>Login Failed</title>\n"
            "<style>\n"
            "body { font-family: Arial, sans-serif; max-width: 400px; margin: 50px auto; }\n"
            ".error { color: red; background: #ffeeee; padding: 10px; border-radius: 5px; }\n"
            "</style>\n"
            "</head>\n"
            "<body>\n"
            "<h1>Login Failed</h1>\n"
            "<div class=\"error\">Invalid username or password.</div>\n"
            "<p><a href=\"/admin/login\">Try Again</a></p>\n"
            "</body>\n"
            "</html>";
        return http_response_create(401, "text/html", html, strlen(html));
    }
    
    const char *html = "<html><body><h1>Method Not Allowed</h1></body></html>";
    return http_response_create(405, "text/html", html, strlen(html));
}

http_response_t *admin_logout_handler(http_request_t *req) {
    if (req->cookies) {
        char *session_token = get_cookie_value(req->cookies, "admin_session");
        if (session_token) {
            auth_destroy_session(session_token);
        }
    }
    
    char *cookie = malloc(128);
    if (cookie) {
        snprintf(cookie, 128, "admin_session=; Path=/; Max-Age=0; HttpOnly");
        
        const char *html = 
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<title>Logged Out</title>\n"
            "<style>\n"
            "body { font-family: Arial, sans-serif; max-width: 400px; margin: 50px auto; }\n"
            "h1 { color: #333; }\n"
            ".message { background: #e8f5e9; padding: 15px; border-radius: 5px; margin: 20px 0; }\n"
            "</style>\n"
            "</head>\n"
            "<body>\n"
            "<h1>Logged Out</h1>\n"
            "<div class=\"message\">You have been successfully logged out.</div>\n"
            "<p><a href=\"/admin/login\">Login Again</a></p>\n"
            "<p><a href=\"/\">Back to Site</a></p>\n"
            "</body>\n"
            "</html>";
        
        http_response_t *response = http_response_create(200, "text/html", html, strlen(html));
        if (response) {
            response->set_cookie = cookie;
        }
        return response;
    }
    
    const char *html = 
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head><title>Logged Out</title></head>\n"
        "<body>\n"
        "<h1>Logged Out</h1>\n"
        "<p>You have been successfully logged out.</p>\n"
        "<p><a href=\"/admin/login\">Login Again</a></p>\n"
        "<p><a href=\"/\">Back to Site</a></p>\n"
        "</body>\n"
        "</html>";
    return http_response_create(200, "text/html", html, strlen(html));
}

static int get_current_user_id(http_request_t *req) {
    if (!req->cookies) {
        return -1;
    }
    
    char *session_token = get_cookie_value(req->cookies, "admin_session");
    if (!session_token) {
        return -1;
    }
    
    sqlite3_stmt *stmt = db_prepare(
        "SELECT s.user_id FROM admin_sessions s "
        "WHERE s.token = ? AND s.expires_at > datetime('now')"
    );
    
    if (!stmt) {
        return -1;
    }
    
    sqlite3_bind_text(stmt, 1, session_token, -1, SQLITE_STATIC);
    
    int user_id = -1;
    if (db_step(stmt) == SQLITE_ROW) {
        user_id = sqlite3_column_int(stmt, 0);
    }
    
    db_finalize(stmt);
    return user_id;
}

http_response_t *admin_change_password_handler(http_request_t *req) {
    if (!admin_is_authenticated(req)) {
        const char *html = 
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head><title>Access Denied</title></head>\n"
            "<body>\n"
            "<h1>Access Denied</h1>\n"
            "<p>You must be logged in to access this page.</p>\n"
            "<p><a href=\"/admin/login\">Login</a></p>\n"
            "</body>\n"
            "</html>";
        return http_response_create(403, "text/html", html, strlen(html));
    }
    
    if (strcmp(req->method, "GET") == 0) {
        const char *html = 
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<title>Change Password</title>\n"
            "<style>\n"
            "body { font-family: Arial, sans-serif; max-width: 500px; margin: 50px auto; }\n"
            "h1 { color: #333; }\n"
            "form { background: #f5f5f5; padding: 20px; border-radius: 5px; }\n"
            "label { display: block; margin-top: 10px; }\n"
            "input[type=\"password\"] { width: 100%; padding: 8px; margin-top: 5px; }\n"
            "input[type=\"submit\"] { margin-top: 15px; padding: 10px 20px; "
            "background: #4CAF50; color: white; border: none; cursor: pointer; }\n"
            "input[type=\"submit\"]:hover { background: #45a049; }\n"
            "</style>\n"
            "</head>\n"
            "<body>\n"
            "<h1>Change Password</h1>\n"
            "<form method=\"POST\" action=\"/admin/change-password\">\n"
            "<label for=\"current_password\">Current Password:</label>\n"
            "<input type=\"password\" id=\"current_password\" name=\"current_password\" required>\n"
            "<label for=\"new_password\">New Password:</label>\n"
            "<input type=\"password\" id=\"new_password\" name=\"new_password\" required>\n"
            "<label for=\"confirm_password\">Confirm New Password:</label>\n"
            "<input type=\"password\" id=\"confirm_password\" name=\"confirm_password\" required>\n"
            "<input type=\"submit\" value=\"Change Password\">\n"
            "</form>\n"
            "<p><a href=\"/admin\">Back to Dashboard</a></p>\n"
            "</body>\n"
            "</html>";
        return http_response_create(200, "text/html", html, strlen(html));
    } else if (strcmp(req->method, "POST") == 0) {
        if (!req->body) {
            const char *html = "<html><body><h1>Bad Request</h1></body></html>";
            return http_response_create(400, "text/html", html, strlen(html));
        }
        
        int user_id = get_current_user_id(req);
        if (user_id < 0) {
            const char *html = "<html><body><h1>Session Error</h1></body></html>";
            return http_response_create(401, "text/html", html, strlen(html));
        }
        
        char current_password[128] = {0};
        char new_password[128] = {0};
        char confirm_password[128] = {0};
        
        char *curr_param = strstr(req->body, "current_password=");
        char *new_param = strstr(req->body, "new_password=");
        char *conf_param = strstr(req->body, "confirm_password=");
        
        if (curr_param) {
            curr_param += 17;
            char *end = strchr(curr_param, '&');
            size_t len = end ? (size_t)(end - curr_param) : strlen(curr_param);
            if (len < sizeof(current_password)) {
                memcpy(current_password, curr_param, len);
                current_password[len] = '\0';
            }
        }
        
        if (new_param) {
            new_param += 13;
            char *end = strchr(new_param, '&');
            size_t len = end ? (size_t)(end - new_param) : strlen(new_param);
            if (len < sizeof(new_password)) {
                memcpy(new_password, new_param, len);
                new_password[len] = '\0';
            }
        }
        
        if (conf_param) {
            conf_param += 17;
            char *end = strchr(conf_param, '&');
            size_t len = end ? (size_t)(end - conf_param) : strlen(conf_param);
            if (len < sizeof(confirm_password)) {
                memcpy(confirm_password, conf_param, len);
                confirm_password[len] = '\0';
            }
        }
        
        if (strcmp(new_password, confirm_password) != 0) {
            const char *html = 
                "<!DOCTYPE html>\n"
                "<html>\n"
                "<head><title>Password Change Failed</title></head>\n"
                "<body>\n"
                "<h1>Password Change Failed</h1>\n"
                "<p>New passwords do not match.</p>\n"
                "<p><a href=\"/admin/change-password\">Try Again</a></p>\n"
                "</body>\n"
                "</html>";
            return http_response_create(400, "text/html", html, strlen(html));
        }
        
        sqlite3_stmt *stmt = db_prepare(
            "SELECT id FROM admin_users WHERE id = ? AND password = ?"
        );
        
        if (!stmt) {
            const char *html = "<html><body><h1>Server Error</h1></body></html>";
            return http_response_create(500, "text/html", html, strlen(html));
        }
        
        sqlite3_bind_int(stmt, 1, user_id);
        sqlite3_bind_text(stmt, 2, current_password, -1, SQLITE_STATIC);
        
        int valid = 0;
        if (db_step(stmt) == SQLITE_ROW) {
            valid = 1;
        }
        db_finalize(stmt);
        
        if (!valid) {
            const char *html = 
                "<!DOCTYPE html>\n"
                "<html>\n"
                "<head><title>Password Change Failed</title></head>\n"
                "<body>\n"
                "<h1>Password Change Failed</h1>\n"
                "<p>Current password is incorrect.</p>\n"
                "<p><a href=\"/admin/change-password\">Try Again</a></p>\n"
                "</body>\n"
                "</html>";
            return http_response_create(401, "text/html", html, strlen(html));
        }
        
        stmt = db_prepare("UPDATE admin_users SET password = ? WHERE id = ?");
        if (!stmt) {
            const char *html = "<html><body><h1>Server Error</h1></body></html>";
            return http_response_create(500, "text/html", html, strlen(html));
        }
        
        sqlite3_bind_text(stmt, 1, new_password, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, user_id);
        
        if (db_step(stmt) != SQLITE_DONE) {
            db_finalize(stmt);
            const char *html = "<html><body><h1>Failed to Update Password</h1></body></html>";
            return http_response_create(500, "text/html", html, strlen(html));
        }
        db_finalize(stmt);
        
        const char *html = 
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head><title>Password Changed</title></head>\n"
            "<body>\n"
            "<h1>Password Changed Successfully</h1>\n"
            "<p>Your password has been updated.</p>\n"
            "<p><a href=\"/admin\">Back to Dashboard</a></p>\n"
            "</body>\n"
            "</html>";
        return http_response_create(200, "text/html", html, strlen(html));
    }
    
    const char *html = "<html><body><h1>Method Not Allowed</h1></body></html>";
    return http_response_create(405, "text/html", html, strlen(html));
}
