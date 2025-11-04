#include "auth.h"
#include "db.h"
#include "utils.h"
#include <string.h>

int auth_is_authenticated(http_request_t *req) {
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

char *auth_create_session(int user_id) {
    char *token = generate_random_token(64);
    
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

void auth_destroy_session(const char *token) {
    sqlite3_stmt *stmt = db_prepare("DELETE FROM admin_sessions WHERE token = ?");
    if (stmt) {
        sqlite3_bind_text(stmt, 1, token, -1, SQLITE_STATIC);
        db_step(stmt);
        db_finalize(stmt);
    }
}
