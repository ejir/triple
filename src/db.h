#ifndef DB_H
#define DB_H

#include <stddef.h>
#include <stdint.h>

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

int db_init(const char *db_path);
void db_close(void);
sqlite3 *db_get_connection(void);

int db_exec(const char *sql);
sqlite3_stmt *db_prepare(const char *sql);
int db_step(sqlite3_stmt *stmt);
void db_finalize(sqlite3_stmt *stmt);

int db_migrate(void);

#endif
