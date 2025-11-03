#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#include "db.h"

#define TEST_DB_PATH "test_db_wrapper.db"

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

void test_db_init_close(void) {
    test_start("DB wrapper init and close");
    
    cleanup_test_db();
    
    int rc = db_init(TEST_DB_PATH);
    if (rc != 0) {
        test_fail("db_init failed");
        return;
    }
    
    sqlite3 *conn = db_get_connection();
    if (conn == NULL) {
        test_fail("db_get_connection returned NULL");
        db_close();
        cleanup_test_db();
        return;
    }
    
    printf("  Database connection established\n");
    
    db_close();
    cleanup_test_db();
    test_pass();
}

void test_db_exec(void) {
    test_start("DB wrapper exec");
    
    cleanup_test_db();
    
    int rc = db_init(TEST_DB_PATH);
    assert(rc == 0);
    
    rc = db_exec("CREATE TABLE test (id INTEGER PRIMARY KEY, value TEXT);");
    if (rc != 0) {
        test_fail("db_exec failed to create table");
        db_close();
        cleanup_test_db();
        return;
    }
    
    printf("  Table created successfully\n");
    
    rc = db_exec("INSERT INTO test (value) VALUES ('test data');");
    if (rc != 0) {
        test_fail("db_exec failed to insert data");
        db_close();
        cleanup_test_db();
        return;
    }
    
    printf("  Data inserted successfully\n");
    
    db_close();
    cleanup_test_db();
    test_pass();
}

void test_db_prepare_step(void) {
    test_start("DB wrapper prepare and step");
    
    cleanup_test_db();
    
    int rc = db_init(TEST_DB_PATH);
    assert(rc == 0);
    
    rc = db_exec("CREATE TABLE items (id INTEGER PRIMARY KEY, name TEXT);");
    assert(rc == 0);
    
    rc = db_exec("INSERT INTO items (name) VALUES ('Item 1'), ('Item 2'), ('Item 3');");
    assert(rc == 0);
    
    sqlite3_stmt *stmt = db_prepare("SELECT id, name FROM items ORDER BY id;");
    if (stmt == NULL) {
        test_fail("db_prepare failed");
        db_close();
        cleanup_test_db();
        return;
    }
    
    printf("  Statement prepared successfully\n");
    printf("  Query results:\n");
    
    int row_count = 0;
    while (db_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        printf("    ID: %d, Name: %s\n", id, name);
        row_count++;
    }
    
    db_finalize(stmt);
    db_close();
    cleanup_test_db();
    
    if (row_count == 3) {
        test_pass();
    } else {
        test_fail("Unexpected row count");
    }
}

void test_db_migrate(void) {
    test_start("DB wrapper migrate");
    
    cleanup_test_db();
    
    int rc = db_init(TEST_DB_PATH);
    assert(rc == 0);
    
    rc = db_migrate();
    if (rc != 0) {
        test_fail("db_migrate failed");
        db_close();
        cleanup_test_db();
        return;
    }
    
    printf("  Migrations run successfully\n");
    
    sqlite3_stmt *stmt = db_prepare(
        "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;"
    );
    assert(stmt != NULL);
    
    printf("  Tables created:\n");
    int table_count = 0;
    while (db_step(stmt) == SQLITE_ROW) {
        const char *name = (const char *)sqlite3_column_text(stmt, 0);
        printf("    - %s\n", name);
        table_count++;
    }
    
    db_finalize(stmt);
    db_close();
    cleanup_test_db();
    
    if (table_count >= 3) {
        test_pass();
    } else {
        test_fail("Expected at least 3 tables");
    }
}

void test_db_full_workflow(void) {
    test_start("DB wrapper full workflow");
    
    cleanup_test_db();
    
    int rc = db_init(TEST_DB_PATH);
    assert(rc == 0);
    
    rc = db_migrate();
    assert(rc == 0);
    
    rc = db_exec("INSERT INTO boards (name, title, description) VALUES ('tech', 'Technology', 'Tech discussions');");
    if (rc != 0) {
        test_fail("Failed to insert board");
        db_close();
        cleanup_test_db();
        return;
    }
    
    printf("  Board created\n");
    
    sqlite3_stmt *stmt = db_prepare("SELECT id FROM boards WHERE name = 'tech';");
    assert(stmt != NULL);
    
    int board_id = -1;
    if (db_step(stmt) == SQLITE_ROW) {
        board_id = sqlite3_column_int(stmt, 0);
        printf("  Board ID: %d\n", board_id);
    }
    db_finalize(stmt);
    
    if (board_id <= 0) {
        test_fail("Failed to retrieve board ID");
        db_close();
        cleanup_test_db();
        return;
    }
    
    char insert_thread_sql[256];
    snprintf(insert_thread_sql, sizeof(insert_thread_sql),
             "INSERT INTO threads (board_id, subject) VALUES (%d, 'First Thread');",
             board_id);
    rc = db_exec(insert_thread_sql);
    assert(rc == 0);
    
    printf("  Thread created\n");
    
    stmt = db_prepare("SELECT COUNT(*) FROM threads;");
    assert(stmt != NULL);
    
    int thread_count = 0;
    if (db_step(stmt) == SQLITE_ROW) {
        thread_count = sqlite3_column_int(stmt, 0);
        printf("  Thread count: %d\n", thread_count);
    }
    db_finalize(stmt);
    
    db_close();
    cleanup_test_db();
    
    if (thread_count == 1) {
        test_pass();
    } else {
        test_fail("Unexpected thread count");
    }
}

void test_db_error_handling(void) {
    test_start("DB wrapper error handling");
    
    cleanup_test_db();
    
    sqlite3_stmt *stmt = db_prepare("SELECT * FROM nonexistent;");
    if (stmt != NULL) {
        test_fail("db_prepare should fail without initialized connection");
        db_finalize(stmt);
        return;
    }
    
    printf("  Correctly handles uninitialized connection\n");
    
    int rc = db_init(TEST_DB_PATH);
    assert(rc == 0);
    
    rc = db_exec("INVALID SQL SYNTAX");
    if (rc == 0) {
        test_fail("db_exec should fail with invalid SQL");
        db_close();
        cleanup_test_db();
        return;
    }
    
    printf("  Correctly handles invalid SQL\n");
    
    db_close();
    cleanup_test_db();
    test_pass();
}

int main(void) {
    printf("\n");
    printf("======================================\n");
    printf("  DB Wrapper Test Suite\n");
    printf("======================================\n\n");
    
    test_db_init_close();
    test_db_exec();
    test_db_prepare_step();
    test_db_migrate();
    test_db_full_workflow();
    test_db_error_handling();
    
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
