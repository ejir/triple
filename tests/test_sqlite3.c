#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "sqlite3.h"

#define TEST_DB_PATH "test_sqlite3.db"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static int test_count = 0;
static int test_passed = 0;
static int test_failed = 0;

void cleanup_test_db(void) {
    unlink(TEST_DB_PATH);
}

void test_start(const char *name) {
    test_count++;
    printf(ANSI_COLOR_YELLOW "[TEST %d] %s" ANSI_COLOR_RESET "\n", test_count, name);
}

void test_pass(void) {
    test_passed++;
    printf(ANSI_COLOR_GREEN "  ✓ PASSED" ANSI_COLOR_RESET "\n\n");
}

void test_fail(const char *msg) {
    test_failed++;
    printf(ANSI_COLOR_RED "  ✗ FAILED: %s" ANSI_COLOR_RESET "\n\n", msg);
}

void test_sqlite3_version(void) {
    test_start("SQLite3 version check");
    
    const char *version = sqlite3_libversion();
    printf("  SQLite version: %s\n", version);
    
    if (version != NULL && strlen(version) > 0) {
        test_pass();
    } else {
        test_fail("Invalid version string");
    }
}

void test_sqlite3_open_close(void) {
    test_start("SQLite3 open and close database");
    
    cleanup_test_db();
    
    sqlite3 *db = NULL;
    int rc = sqlite3_open(TEST_DB_PATH, &db);
    
    if (rc != SQLITE_OK) {
        test_fail("Failed to open database");
        return;
    }
    
    printf("  Database opened successfully\n");
    
    rc = sqlite3_close(db);
    if (rc != SQLITE_OK) {
        test_fail("Failed to close database");
        return;
    }
    
    printf("  Database closed successfully\n");
    cleanup_test_db();
    test_pass();
}

void test_sqlite3_create_table(void) {
    test_start("SQLite3 create table");
    
    cleanup_test_db();
    
    sqlite3 *db = NULL;
    int rc = sqlite3_open(TEST_DB_PATH, &db);
    assert(rc == SQLITE_OK);
    
    const char *sql = 
        "CREATE TABLE users ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT NOT NULL,"
        "    email TEXT UNIQUE,"
        "    created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";
    
    char *err_msg = NULL;
    rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
    
    if (rc != SQLITE_OK) {
        printf("  Error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        cleanup_test_db();
        test_fail("Failed to create table");
        return;
    }
    
    printf("  Table created successfully\n");
    
    sqlite3_close(db);
    cleanup_test_db();
    test_pass();
}

void test_sqlite3_insert_data(void) {
    test_start("SQLite3 insert data");
    
    cleanup_test_db();
    
    sqlite3 *db = NULL;
    int rc = sqlite3_open(TEST_DB_PATH, &db);
    assert(rc == SQLITE_OK);
    
    const char *create_sql = 
        "CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, email TEXT);";
    char *err_msg = NULL;
    rc = sqlite3_exec(db, create_sql, NULL, NULL, &err_msg);
    assert(rc == SQLITE_OK);
    
    const char *insert_sql = 
        "INSERT INTO users (name, email) VALUES ('Alice', 'alice@example.com');"
        "INSERT INTO users (name, email) VALUES ('Bob', 'bob@example.com');";
    
    rc = sqlite3_exec(db, insert_sql, NULL, NULL, &err_msg);
    
    if (rc != SQLITE_OK) {
        printf("  Error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        cleanup_test_db();
        test_fail("Failed to insert data");
        return;
    }
    
    printf("  Data inserted successfully\n");
    
    const char *count_sql = "SELECT COUNT(*) FROM users;";
    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, count_sql, -1, &stmt, NULL);
    assert(rc == SQLITE_OK);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        test_fail("Failed to query data");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        cleanup_test_db();
        return;
    }
    
    int count = sqlite3_column_int(stmt, 0);
    printf("  Row count: %d\n", count);
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    cleanup_test_db();
    
    if (count == 2) {
        test_pass();
    } else {
        test_fail("Unexpected row count");
    }
}

void test_sqlite3_query_data(void) {
    test_start("SQLite3 query data");
    
    cleanup_test_db();
    
    sqlite3 *db = NULL;
    int rc = sqlite3_open(TEST_DB_PATH, &db);
    assert(rc == SQLITE_OK);
    
    const char *create_sql = 
        "CREATE TABLE products (id INTEGER PRIMARY KEY, name TEXT, price REAL);";
    char *err_msg = NULL;
    rc = sqlite3_exec(db, create_sql, NULL, NULL, &err_msg);
    assert(rc == SQLITE_OK);
    
    const char *insert_sql = 
        "INSERT INTO products (name, price) VALUES ('Laptop', 999.99);"
        "INSERT INTO products (name, price) VALUES ('Mouse', 19.99);"
        "INSERT INTO products (name, price) VALUES ('Keyboard', 49.99);";
    rc = sqlite3_exec(db, insert_sql, NULL, NULL, &err_msg);
    assert(rc == SQLITE_OK);
    
    const char *query_sql = "SELECT id, name, price FROM products ORDER BY id;";
    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, query_sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        test_fail("Failed to prepare query");
        sqlite3_close(db);
        cleanup_test_db();
        return;
    }
    
    printf("  Query results:\n");
    int row_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        double price = sqlite3_column_double(stmt, 2);
        printf("    ID: %d, Name: %s, Price: $%.2f\n", id, name, price);
        row_count++;
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    cleanup_test_db();
    
    if (row_count == 3) {
        test_pass();
    } else {
        test_fail("Unexpected row count");
    }
}

void test_sqlite3_prepared_statements(void) {
    test_start("SQLite3 prepared statements with parameters");
    
    cleanup_test_db();
    
    sqlite3 *db = NULL;
    int rc = sqlite3_open(TEST_DB_PATH, &db);
    assert(rc == SQLITE_OK);
    
    const char *create_sql = 
        "CREATE TABLE books (id INTEGER PRIMARY KEY, title TEXT, author TEXT, year INTEGER);";
    char *err_msg = NULL;
    rc = sqlite3_exec(db, create_sql, NULL, NULL, &err_msg);
    assert(rc == SQLITE_OK);
    
    const char *insert_sql = "INSERT INTO books (title, author, year) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL);
    
    if (rc != SQLITE_OK) {
        test_fail("Failed to prepare insert statement");
        sqlite3_close(db);
        cleanup_test_db();
        return;
    }
    
    sqlite3_bind_text(stmt, 1, "The C Programming Language", -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, "Kernighan & Ritchie", -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, 1978);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        test_fail("Failed to execute insert");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        cleanup_test_db();
        return;
    }
    
    printf("  Inserted book using prepared statement\n");
    sqlite3_finalize(stmt);
    
    const char *select_sql = "SELECT title, author, year FROM books WHERE year = ?;";
    rc = sqlite3_prepare_v2(db, select_sql, -1, &stmt, NULL);
    assert(rc == SQLITE_OK);
    
    sqlite3_bind_int(stmt, 1, 1978);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *title = (const char *)sqlite3_column_text(stmt, 0);
        const char *author = (const char *)sqlite3_column_text(stmt, 1);
        int year = sqlite3_column_int(stmt, 2);
        printf("  Found: %s by %s (%d)\n", title, author, year);
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    cleanup_test_db();
    test_pass();
}

void test_sqlite3_transactions(void) {
    test_start("SQLite3 transactions");
    
    cleanup_test_db();
    
    sqlite3 *db = NULL;
    int rc = sqlite3_open(TEST_DB_PATH, &db);
    assert(rc == SQLITE_OK);
    
    const char *create_sql = 
        "CREATE TABLE accounts (id INTEGER PRIMARY KEY, name TEXT, balance REAL);";
    char *err_msg = NULL;
    rc = sqlite3_exec(db, create_sql, NULL, NULL, &err_msg);
    assert(rc == SQLITE_OK);
    
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &err_msg);
    assert(rc == SQLITE_OK);
    
    const char *insert_sql = 
        "INSERT INTO accounts (name, balance) VALUES ('Alice', 1000.0);"
        "INSERT INTO accounts (name, balance) VALUES ('Bob', 500.0);";
    rc = sqlite3_exec(db, insert_sql, NULL, NULL, &err_msg);
    
    if (rc != SQLITE_OK) {
        sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
        test_fail("Failed to insert in transaction");
        sqlite3_close(db);
        cleanup_test_db();
        return;
    }
    
    rc = sqlite3_exec(db, "COMMIT;", NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        test_fail("Failed to commit transaction");
        sqlite3_close(db);
        cleanup_test_db();
        return;
    }
    
    printf("  Transaction committed successfully\n");
    
    const char *count_sql = "SELECT COUNT(*) FROM accounts;";
    sqlite3_stmt *stmt = NULL;
    rc = sqlite3_prepare_v2(db, count_sql, -1, &stmt, NULL);
    assert(rc == SQLITE_OK);
    
    rc = sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    printf("  Account count: %d\n", count);
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    cleanup_test_db();
    
    if (count == 2) {
        test_pass();
    } else {
        test_fail("Transaction did not commit properly");
    }
}

int main(void) {
    printf("\n");
    printf("======================================\n");
    printf("  SQLite3 Integration Test Suite\n");
    printf("======================================\n\n");
    
    test_sqlite3_version();
    test_sqlite3_open_close();
    test_sqlite3_create_table();
    test_sqlite3_insert_data();
    test_sqlite3_query_data();
    test_sqlite3_prepared_statements();
    test_sqlite3_transactions();
    
    printf("======================================\n");
    printf("  Test Summary\n");
    printf("======================================\n");
    printf("Total tests:  %d\n", test_count);
    printf(ANSI_COLOR_GREEN "Passed:       %d" ANSI_COLOR_RESET "\n", test_passed);
    if (test_failed > 0) {
        printf(ANSI_COLOR_RED "Failed:       %d" ANSI_COLOR_RESET "\n", test_failed);
    } else {
        printf("Failed:       %d\n", test_failed);
    }
    printf("======================================\n\n");
    
    if (test_failed > 0) {
        return 1;
    }
    
    return 0;
}
