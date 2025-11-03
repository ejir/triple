#define _POSIX_C_SOURCE 200809L
#include "board.h"
#include "router.h"
#include "render.h"
#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    router_add_route("GET", "/thread", thread_view_handler);
    router_add_route("POST", "/thread", thread_create_handler);
    router_add_route("POST", "/post", post_create_handler);
}

http_response_t *board_list_handler(http_request_t *req) {
    (void)req;
    char *html = malloc(8192);
    if (!html) {
        const char *err = "<html><body><h1>Error: Out of memory</h1></body></html>";
        return http_response_create(500, "text/html", err, strlen(err));
    }
    
    int len = snprintf(html, 8192,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head><title>Boards</title></head>\n"
        "<body>\n"
        "<h1>Message Boards</h1>\n"
        "<ul>\n");
    
    sqlite3_stmt *stmt = db_prepare("SELECT id, name, title, description FROM boards ORDER BY name");
    if (stmt) {
        while (db_step(stmt) == SQLITE_ROW) {
            int64_t id = sqlite3_column_int64(stmt, 0);
            const char *name = (const char *)sqlite3_column_text(stmt, 1);
            const char *title = (const char *)sqlite3_column_text(stmt, 2);
            const char *desc = (const char *)sqlite3_column_text(stmt, 3);
            
            len += snprintf(html + len, 8192 - len,
                "<li><a href=\"/board?id=%lld\">%s - %s</a><br><small>%s</small></li>\n",
                (long long)id, 
                name ? name : "Unknown",
                title ? title : "No Title",
                desc ? desc : "");
        }
        db_finalize(stmt);
    }
    
    len += snprintf(html + len, 8192 - len,
        "</ul>\n"
        "<hr>\n"
        "<h2>Create New Board</h2>\n"
        "<form method=\"POST\" action=\"/board/create\">\n"
        "Name: <input type=\"text\" name=\"name\" required><br>\n"
        "Title: <input type=\"text\" name=\"title\" required><br>\n"
        "Description: <textarea name=\"description\"></textarea><br>\n"
        "<button type=\"submit\">Create Board</button>\n"
        "</form>\n"
        "</body>\n"
        "</html>");
    
    http_response_t *response = http_response_create(200, "text/html", html, len);
    free(html);
    return response;
}

http_response_t *board_view_handler(http_request_t *req) {
    int64_t board_id = 1;
    if (req->query_string) {
        sscanf(req->query_string, "id=%lld", (long long *)&board_id);
    }
    
    board_t *board = board_get_by_id(board_id);
    if (!board) {
        const char *html = "<html><body><h1>Board Not Found</h1><a href=\"/\">Back to boards</a></body></html>";
        return http_response_create(404, "text/html", html, strlen(html));
    }
    
    char *html = malloc(16384);
    if (!html) {
        board_free(board);
        const char *err = "<html><body><h1>Error: Out of memory</h1></body></html>";
        return http_response_create(500, "text/html", err, strlen(err));
    }
    
    int len = snprintf(html, 16384,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head><title>%s</title></head>\n"
        "<body>\n"
        "<h1>/%s/ - %s</h1>\n"
        "<p>%s</p>\n"
        "<a href=\"/\">Back to boards</a><hr>\n"
        "<h2>Threads</h2>\n"
        "<ul>\n",
        board->name ? board->name : "Board",
        board->name ? board->name : "board",
        board->name ? board->name : "Board",
        board->description ? board->description : "No description");
    
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
            
            len += snprintf(html + len, 16384 - len,
                "<li><a href=\"/thread?id=%lld\">%s</a> (%d posts)</li>\n",
                (long long)thread_id,
                subject ? subject : "No Subject",
                post_count);
        }
        db_finalize(stmt);
    }
    
    len += snprintf(html + len, 16384 - len,
        "</ul>\n"
        "<hr>\n"
        "<h2>Create New Thread</h2>\n"
        "<form method=\"POST\" action=\"/thread\">\n"
        "<input type=\"hidden\" name=\"board_id\" value=\"%lld\">\n"
        "Subject: <input type=\"text\" name=\"subject\" required><br>\n"
        "Name: <input type=\"text\" name=\"author\" placeholder=\"Anonymous\"><br>\n"
        "Content: <textarea name=\"content\" required></textarea><br>\n"
        "<button type=\"submit\">Create Thread</button>\n"
        "</form>\n"
        "</body>\n"
        "</html>",
        (long long)board_id);
    
    board_free(board);
    http_response_t *response = http_response_create(200, "text/html", html, len);
    free(html);
    return response;
}

http_response_t *thread_view_handler(http_request_t *req) {
    int64_t thread_id = 1;
    if (req->query_string) {
        sscanf(req->query_string, "id=%lld", (long long *)&thread_id);
    }
    
    thread_t *thread = thread_get_by_id(thread_id);
    if (!thread) {
        const char *html = "<html><body><h1>Thread Not Found</h1><a href=\"/\">Back to boards</a></body></html>";
        return http_response_create(404, "text/html", html, strlen(html));
    }
    
    char *html = malloc(32768);
    if (!html) {
        thread_free(thread);
        const char *err = "<html><body><h1>Error: Out of memory</h1></body></html>";
        return http_response_create(500, "text/html", err, strlen(err));
    }
    
    int len = snprintf(html, 32768,
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head><title>%s</title></head>\n"
        "<body>\n"
        "<h1>%s</h1>\n"
        "<a href=\"/board?id=%lld\">Back to board</a> | <a href=\"/\">All boards</a><hr>\n"
        "<div style=\"border:1px solid #ccc; padding:10px; margin:10px 0;\">\n"
        "<p><strong>%s</strong></p>\n"
        "<p>%s</p>\n"
        "</div>\n"
        "<h2>Replies</h2>\n",
        thread->subject ? thread->subject : "Thread",
        thread->subject ? thread->subject : "Thread",
        (long long)thread->board_id,
        thread->author ? thread->author : "Anonymous",
        thread->content ? thread->content : "No content");
    
    sqlite3_stmt *stmt = db_prepare(
        "SELECT id, author, content, created_at FROM posts "
        "WHERE thread_id = ? ORDER BY created_at ASC"
    );
    
    if (stmt) {
        sqlite3_bind_int64(stmt, 1, thread_id);
        
        while (db_step(stmt) == SQLITE_ROW) {
            int64_t post_id = sqlite3_column_int64(stmt, 0);
            const char *author = (const char *)sqlite3_column_text(stmt, 1);
            const char *content = (const char *)sqlite3_column_text(stmt, 2);
            
            len += snprintf(html + len, 32768 - len,
                "<div style=\"border:1px solid #ddd; padding:8px; margin:8px 0;\">\n"
                "<p><strong>%s</strong> (#%lld)</p>\n"
                "<p>%s</p>\n"
                "</div>\n",
                author ? author : "Anonymous",
                (long long)post_id,
                content ? content : "");
        }
        db_finalize(stmt);
    }
    
    len += snprintf(html + len, 32768 - len,
        "<hr>\n"
        "<h2>Reply to Thread</h2>\n"
        "<form method=\"POST\" action=\"/post\">\n"
        "<input type=\"hidden\" name=\"thread_id\" value=\"%lld\">\n"
        "Name: <input type=\"text\" name=\"author\" placeholder=\"Anonymous\"><br>\n"
        "Content: <textarea name=\"content\" required></textarea><br>\n"
        "<button type=\"submit\">Post Reply</button>\n"
        "</form>\n"
        "</body>\n"
        "</html>",
        (long long)thread_id);
    
    thread_free(thread);
    http_response_t *response = http_response_create(200, "text/html", html, len);
    free(html);
    return response;
}

http_response_t *thread_create_handler(http_request_t *req) {
    if (!req->body) {
        const char *html = "<html><body><h1>Error: No form data</h1></body></html>";
        return http_response_create(400, "text/html", html, strlen(html));
    }
    
    int64_t board_id = 1;
    char subject[256] = {0};
    char author[128] = "Anonymous";
    char content[2048] = {0};
    
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
            
            if (strcmp(key, "board_id") == 0) {
                board_id = atoll(value);
            } else if (strcmp(key, "subject") == 0) {
                strncpy(subject, value, sizeof(subject) - 1);
            } else if (strcmp(key, "author") == 0 && strlen(value) > 0) {
                strncpy(author, value, sizeof(author) - 1);
            } else if (strcmp(key, "content") == 0) {
                strncpy(content, value, sizeof(content) - 1);
            }
        }
        token = strtok(NULL, "&");
    }
    free(body_copy);
    
    sqlite3_stmt *stmt = db_prepare(
        "INSERT INTO threads (board_id, subject) VALUES (?, ?)"
    );
    if (!stmt) {
        const char *html = "<html><body><h1>Error: Failed to create thread</h1></body></html>";
        return http_response_create(500, "text/html", html, strlen(html));
    }
    
    sqlite3_bind_int64(stmt, 1, board_id);
    sqlite3_bind_text(stmt, 2, subject, -1, SQLITE_TRANSIENT);
    
    int rc = db_step(stmt);
    int64_t thread_id = sqlite3_last_insert_rowid(db_get_connection());
    db_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        const char *html = "<html><body><h1>Error: Failed to create thread</h1></body></html>";
        return http_response_create(500, "text/html", html, strlen(html));
    }
    
    sqlite3_stmt *post_stmt = db_prepare(
        "INSERT INTO posts (thread_id, author, content) VALUES (?, ?, ?)"
    );
    if (post_stmt) {
        sqlite3_bind_int64(post_stmt, 1, thread_id);
        sqlite3_bind_text(post_stmt, 2, author, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(post_stmt, 3, content, -1, SQLITE_TRANSIENT);
        db_step(post_stmt);
        db_finalize(post_stmt);
    }
    
    char *html = malloc(512);
    if (!html) {
        const char *err = "<html><body><h1>Error: Out of memory</h1></body></html>";
        return http_response_create(500, "text/html", err, strlen(err));
    }
    
    int len = snprintf(html, 512,
        "<html><body><h1>Thread Created!</h1>"
        "<p>Your thread has been created.</p>"
        "<a href=\"/thread?id=%lld\">View Thread</a> | "
        "<a href=\"/board?id=%lld\">Back to Board</a></body></html>",
        (long long)thread_id,
        (long long)board_id);
    
    http_response_t *response = http_response_create(200, "text/html", html, len);
    free(html);
    return response;
}

http_response_t *post_create_handler(http_request_t *req) {
    if (!req->body) {
        const char *html = "<html><body><h1>Error: No form data</h1></body></html>";
        return http_response_create(400, "text/html", html, strlen(html));
    }
    
    int64_t thread_id = 0;
    char author[128] = "Anonymous";
    char content[2048] = {0};
    
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
            
            if (strcmp(key, "thread_id") == 0) {
                thread_id = atoll(value);
            } else if (strcmp(key, "author") == 0 && strlen(value) > 0) {
                strncpy(author, value, sizeof(author) - 1);
            } else if (strcmp(key, "content") == 0) {
                strncpy(content, value, sizeof(content) - 1);
            }
        }
        token = strtok(NULL, "&");
    }
    free(body_copy);
    
    if (thread_id == 0) {
        const char *html = "<html><body><h1>Error: Invalid thread ID</h1></body></html>";
        return http_response_create(400, "text/html", html, strlen(html));
    }
    
    sqlite3_stmt *stmt = db_prepare(
        "INSERT INTO posts (thread_id, author, content) VALUES (?, ?, ?)"
    );
    if (!stmt) {
        const char *html = "<html><body><h1>Error: Failed to create post</h1></body></html>";
        return http_response_create(500, "text/html", html, strlen(html));
    }
    
    sqlite3_bind_int64(stmt, 1, thread_id);
    sqlite3_bind_text(stmt, 2, author, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, content, -1, SQLITE_TRANSIENT);
    
    int rc = db_step(stmt);
    db_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        const char *html = "<html><body><h1>Error: Failed to create post</h1></body></html>";
        return http_response_create(500, "text/html", html, strlen(html));
    }
    
    char *html = malloc(512);
    if (!html) {
        const char *err = "<html><body><h1>Error: Out of memory</h1></body></html>";
        return http_response_create(500, "text/html", err, strlen(err));
    }
    
    int len = snprintf(html, 512,
        "<html><body><h1>Post Created!</h1>"
        "<p>Your reply has been posted.</p>"
        "<a href=\"/thread?id=%lld\">Back to Thread</a></body></html>",
        (long long)thread_id);
    
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
