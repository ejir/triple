#include "db.h"
#include <stdio.h>
#include <stdlib.h>

static sqlite3 *db_conn = NULL;

int db_init(const char *db_path) {
    printf("Database initialized: %s (stub)\n", db_path);
    return 0;
}

void db_close(void) {
    if (db_conn) {
        db_conn = NULL;
        printf("Database connection closed\n");
    }
}

sqlite3 *db_get_connection(void) {
    return db_conn;
}

int db_exec(const char *sql) {
    printf("Executing SQL: %s (stub)\n", sql);
    return 0;
}

sqlite3_stmt *db_prepare(const char *sql) {
    printf("Preparing SQL: %s (stub)\n", sql);
    return NULL;
}

int db_step(sqlite3_stmt *stmt) {
    return 0;
}

void db_finalize(sqlite3_stmt *stmt) {
}

int db_migrate(void) {
    printf("Running database migrations (stub)\n");
    return 0;
}
