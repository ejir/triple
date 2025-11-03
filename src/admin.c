#include "admin.h"
#include "router.h"
#include "render.h"
#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    (void)req;
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
        "<a href=\"/\">Back to Site</a><hr>\n"
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
    (void)req;
    const char *html = "<html><body><h1>Admin Login (stub)</h1></body></html>";
    return http_response_create(200, "text/html", html, strlen(html));
}

http_response_t *admin_logout_handler(http_request_t *req) {
    (void)req;
    const char *html = "<html><body><h1>Logged out (stub)</h1></body></html>";
    return http_response_create(200, "text/html", html, strlen(html));
}
