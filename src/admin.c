#include "admin.h"
#include "router.h"
#include "render.h"
#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char *get_cookie_value(const char *cookies, const char *name) {
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

static int is_authenticated(http_request_t *req) {
    if (!req->cookies) {
        return 0;
    }
    
    char *session_token = get_cookie_value(req->cookies, "admin_session");
    if (!session_token) {
        return 0;
    }
    
    sqlite3_stmt *stmt = db_prepare(
        "SELECT s.user_id FROM admin_sessions s "
        "WHERE s.token = ? AND s.expires_at > datetime('now')"
    );
    
    if (!stmt) {
        return 0;
    }
    
    sqlite3_bind_text(stmt, 1, session_token, -1, SQLITE_STATIC);
    
    int authenticated = 0;
    if (db_step(stmt) == SQLITE_ROW) {
        authenticated = 1;
    }
    
    db_finalize(stmt);
    return authenticated;
}

static char *generate_session_token(void) {
    static char token[65];
    static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    
    srand(time(NULL));
    
    for (int i = 0; i < 64; i++) {
        token[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    token[64] = '\0';
    
    return token;
}

static char *create_session(int user_id) {
    char *token = generate_session_token();
    
    sqlite3_stmt *stmt = db_prepare(
        "INSERT INTO admin_sessions (user_id, token, expires_at) "
        "VALUES (?, ?, datetime('now', '+7 days'))"
    );
    
    if (!stmt) {
        return NULL;
    }
    
    sqlite3_bind_int(stmt, 1, user_id);
    sqlite3_bind_text(stmt, 2, token, -1, SQLITE_STATIC);
    
    if (db_step(stmt) != SQLITE_DONE) {
        db_finalize(stmt);
        return NULL;
    }
    
    db_finalize(stmt);
    return token;
}

static void destroy_session(const char *token) {
    sqlite3_stmt *stmt = db_prepare("DELETE FROM admin_sessions WHERE token = ?");
    if (stmt) {
        sqlite3_bind_text(stmt, 1, token, -1, SQLITE_STATIC);
        db_step(stmt);
        db_finalize(stmt);
    }
}

void admin_init(void) {
    printf("Admin module initialized\n");
}

void admin_register_routes(void) {
    router_add_route("GET", "/admin", admin_dashboard_handler);
    router_add_route("GET", "/admin/login", admin_login_handler);
    router_add_route("POST", "/admin/login", admin_login_handler);
    router_add_route("GET", "/admin/logout", admin_logout_handler);
}

http_response_t *admin_dashboard_handler(http_request_t *req) {
    if (!is_authenticated(req)) {
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
        "<head><title>Admin Dashboard</title></head>\n"
        "<body>\n"
        "<h1>Admin Dashboard</h1>\n"
        "<a href=\"/\">Back to Site</a> | <a href=\"/admin/logout\">Logout</a><hr>\n"
        "<h2>Statistics</h2>\n"
        "<ul>\n"
        "<li>Total Boards: %d</li>\n"
        "<li>Total Threads: %d</li>\n"
        "<li>Total Posts: %d</li>\n"
        "</ul>\n"
        "<hr>\n"
        "<h2>Recent Activity</h2>\n"
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
        "</body>\n"
        "</html>");
    
    http_response_t *response = http_response_create(200, "text/html", html, len);
    free(html);
    return response;
}

http_response_t *admin_login_handler(http_request_t *req) {
    if (strcmp(req->method, "GET") == 0) {
        if (is_authenticated(req)) {
            const char *html = 
                "<!DOCTYPE html>\n"
                "<html>\n"
                "<head>\n"
                "<meta http-equiv=\"refresh\" content=\"0; url=/admin\">\n"
                "</head>\n"
                "<body>\n"
                "<p>Already logged in. Redirecting...</p>\n"
                "</body>\n"
                "</html>";
            return http_response_create(200, "text/html", html, strlen(html));
        }
        
        const char *html = 
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<title>Admin Login</title>\n"
            "<style>\n"
            "body { font-family: Arial, sans-serif; max-width: 400px; margin: 50px auto; }\n"
            "h1 { color: #333; }\n"
            "form { background: #f5f5f5; padding: 20px; border-radius: 5px; }\n"
            "label { display: block; margin-top: 10px; }\n"
            "input[type=\"text\"], input[type=\"password\"] { width: 100%; padding: 8px; margin-top: 5px; }\n"
            "input[type=\"submit\"] { margin-top: 15px; padding: 10px 20px; background: #4CAF50; color: white; border: none; cursor: pointer; }\n"
            "input[type=\"submit\"]:hover { background: #45a049; }\n"
            ".error { color: red; margin-top: 10px; }\n"
            "</style>\n"
            "</head>\n"
            "<body>\n"
            "<h1>Admin Login</h1>\n"
            "<form method=\"POST\" action=\"/admin/login\">\n"
            "<label for=\"username\">Username:</label>\n"
            "<input type=\"text\" id=\"username\" name=\"username\" required>\n"
            "<label for=\"password\">Password:</label>\n"
            "<input type=\"password\" id=\"password\" name=\"password\" required>\n"
            "<input type=\"submit\" value=\"Login\">\n"
            "</form>\n"
            "<p><a href=\"/\">Back to Site</a></p>\n"
            "<p><small>Default credentials: admin / admin</small></p>\n"
            "</body>\n"
            "</html>";
        return http_response_create(200, "text/html", html, strlen(html));
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
            char *token = create_session(user_id);
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
            destroy_session(session_token);
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
