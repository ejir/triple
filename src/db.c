#define _POSIX_C_SOURCE 200809L
#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>

static sqlite3 *db_conn = NULL;

static int ensure_directory_exists(const char *path) {
    char *path_copy = strdup(path);
    if (!path_copy) {
        return -1;
    }
    
    char *dir = dirname(path_copy);
    
    struct stat st;
    if (stat(dir, &st) == 0) {
        free(path_copy);
        return 0;
    }
    
    if (mkdir(dir, 0755) != 0 && errno != EEXIST) {
        fprintf(stderr, "Failed to create directory %s: %s\n", dir, strerror(errno));
        free(path_copy);
        return -1;
    }
    
    free(path_copy);
    return 0;
}

int db_init(const char *db_path) {
    if (!db_path || strlen(db_path) == 0) {
        fprintf(stderr, "Invalid database path\n");
        return -1;
    }
    
    if (ensure_directory_exists(db_path) != 0) {
        fprintf(stderr, "Failed to ensure database directory exists\n");
        return -1;
    }
    
    int rc = sqlite3_open(db_path, &db_conn);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db_conn));
        sqlite3_close(db_conn);
        db_conn = NULL;
        return -1;
    }
    
    sqlite3_busy_timeout(db_conn, 5000);
    
    char *err_msg = NULL;
    rc = sqlite3_exec(db_conn, "PRAGMA journal_mode=WAL;", NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to set WAL mode: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    rc = sqlite3_exec(db_conn, "PRAGMA synchronous=NORMAL;", NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to set synchronous mode: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    rc = sqlite3_exec(db_conn, "PRAGMA mmap_size=0;", NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to disable mmap: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    
    printf("Database initialized: %s\n", db_path);
    return 0;
}

void db_close(void) {
    if (db_conn) {
        sqlite3_close(db_conn);
        db_conn = NULL;
        printf("Database connection closed\n");
    }
}

sqlite3 *db_get_connection(void) {
    return db_conn;
}

int db_exec(const char *sql) {
    if (!db_conn) {
        fprintf(stderr, "Database not initialized\n");
        return -1;
    }
    
    char *err_msg = NULL;
    int rc = sqlite3_exec(db_conn, sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }
    return 0;
}

sqlite3_stmt *db_prepare(const char *sql) {
    if (!db_conn) {
        fprintf(stderr, "Database not initialized\n");
        return NULL;
    }
    
    sqlite3_stmt *stmt = NULL;
    int rc = sqlite3_prepare_v2(db_conn, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db_conn));
        return NULL;
    }
    return stmt;
}

int db_step(sqlite3_stmt *stmt) {
    if (!stmt) {
        return SQLITE_ERROR;
    }
    return sqlite3_step(stmt);
}

void db_finalize(sqlite3_stmt *stmt) {
    if (stmt) {
        sqlite3_finalize(stmt);
    }
}

int db_migrate(void) {
    printf("Running database migrations...\n");
    
    const char *create_tables_sql = 
        "CREATE TABLE IF NOT EXISTS boards ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT NOT NULL UNIQUE,"
        "    title TEXT NOT NULL,"
        "    description TEXT,"
        "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");"
        "CREATE TABLE IF NOT EXISTS threads ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    board_id INTEGER NOT NULL,"
        "    subject TEXT NOT NULL,"
        "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    FOREIGN KEY (board_id) REFERENCES boards(id)"
        ");"
        "CREATE TABLE IF NOT EXISTS posts ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    thread_id INTEGER NOT NULL,"
        "    author TEXT,"
        "    content TEXT NOT NULL,"
        "    reply_to INTEGER,"
        "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    FOREIGN KEY (thread_id) REFERENCES threads(id),"
        "    FOREIGN KEY (reply_to) REFERENCES posts(id)"
        ");"
        "CREATE TABLE IF NOT EXISTS admin_users ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    username TEXT NOT NULL UNIQUE,"
        "    password TEXT NOT NULL,"
        "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");"
        "CREATE TABLE IF NOT EXISTS admin_sessions ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    user_id INTEGER NOT NULL,"
        "    token TEXT NOT NULL UNIQUE,"
        "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
        "    expires_at DATETIME NOT NULL,"
        "    FOREIGN KEY (user_id) REFERENCES admin_users(id)"
        ");";
    
    int rc = db_exec(create_tables_sql);
    if (rc != 0) {
        fprintf(stderr, "Failed to run migrations\n");
        return -1;
    }
    
    const char *check_admin_sql = "SELECT COUNT(*) FROM admin_users";
    sqlite3_stmt *stmt = db_prepare(check_admin_sql);
    int admin_count = 0;
    if (stmt) {
        if (db_step(stmt) == SQLITE_ROW) {
            admin_count = sqlite3_column_int(stmt, 0);
        }
        db_finalize(stmt);
    }
    
    if (admin_count == 0) {
        printf("Creating default admin user (username: admin, password: admin)\n");
        const char *insert_admin_sql = 
            "INSERT INTO admin_users (username, password) VALUES ('admin', 'admin')";
        db_exec(insert_admin_sql);
    }
    
    printf("Database migrations completed successfully\n");
    return 0;
}
