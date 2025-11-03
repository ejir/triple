#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/http.h"
#include "../src/router.h"
#include "../src/db.h"
#include "../src/render.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static int test_count = 0;
static int test_passed = 0;
static int test_failed = 0;

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

void test_http_module(void) {
    test_start("HTTP module basic functionality");
    
    http_response_t *response = http_response_create(200, "text/html", "<html></html>", 13);
    if (response == NULL) {
        test_fail("http_response_create failed");
        return;
    }
    printf("  http_response_create: OK\n");
    
    if (response->status_code != 200) {
        http_response_free(response);
        test_fail("status code incorrect");
        return;
    }
    printf("  status_code: %d\n", response->status_code);
    
    if (strcmp(response->content_type, "text/html") != 0) {
        http_response_free(response);
        test_fail("content_type incorrect");
        return;
    }
    printf("  content_type: %s\n", response->content_type);
    
    if (response->body_len != 13) {
        http_response_free(response);
        test_fail("body_len incorrect");
        return;
    }
    printf("  body_len: %zu\n", response->body_len);
    
    http_response_free(response);
    printf("  http_response_free: OK\n");
    
    test_pass();
}

void test_http_response_empty_body(void) {
    test_start("HTTP response with empty body");
    
    http_response_t *response = http_response_create(204, "text/plain", NULL, 0);
    if (response == NULL) {
        test_fail("http_response_create failed");
        return;
    }
    
    if (response->body != NULL) {
        http_response_free(response);
        test_fail("body should be NULL");
        return;
    }
    
    if (response->body_len != 0) {
        http_response_free(response);
        test_fail("body_len should be 0");
        return;
    }
    
    printf("  Empty body handled correctly\n");
    http_response_free(response);
    
    test_pass();
}

http_response_t *test_route_handler(http_request_t *req) {
    (void)req;
    const char *body = "Test response";
    return http_response_create(200, "text/plain", body, strlen(body));
}

void test_router_module(void) {
    test_start("Router module basic functionality");
    
    router_init();
    printf("  router_init: OK\n");
    
    router_add_route("GET", "/test", test_route_handler);
    printf("  router_add_route: OK\n");
    
    http_request_t req = {
        .method = "GET",
        .path = "/test",
        .query_string = NULL,
        .body = NULL,
        .body_len = 0,
        .content_type = NULL
    };
    
    http_response_t *response = router_dispatch(&req);
    if (response == NULL) {
        router_cleanup();
        test_fail("router_dispatch returned NULL");
        return;
    }
    printf("  router_dispatch: OK\n");
    
    if (response->status_code != 200) {
        http_response_free(response);
        router_cleanup();
        test_fail("unexpected status code");
        return;
    }
    
    http_response_free(response);
    router_cleanup();
    printf("  router_cleanup: OK\n");
    
    test_pass();
}

void test_router_not_found(void) {
    test_start("Router 404 handling");
    
    router_init();
    
    http_request_t req = {
        .method = "GET",
        .path = "/nonexistent",
        .query_string = NULL,
        .body = NULL,
        .body_len = 0,
        .content_type = NULL
    };
    
    http_response_t *response = router_dispatch(&req);
    if (response == NULL) {
        router_cleanup();
        test_fail("router_dispatch returned NULL");
        return;
    }
    
    if (response->status_code != 404) {
        http_response_free(response);
        router_cleanup();
        test_fail("expected 404 status code");
        return;
    }
    printf("  404 status: OK\n");
    
    http_response_free(response);
    router_cleanup();
    
    test_pass();
}

void test_render_module(void) {
    test_start("Render module basic functionality");
    
    const char *html = "<html><body>Test</body></html>";
    render_result_t *result = render_html(html, strlen(html));
    
    if (result == NULL) {
        test_fail("render_html failed");
        return;
    }
    printf("  render_html: OK\n");
    
    if (result->len != strlen(html)) {
        render_free(result);
        test_fail("rendered length incorrect");
        return;
    }
    printf("  length: %zu\n", result->len);
    
    if (strcmp(result->html, html) != 0) {
        render_free(result);
        test_fail("rendered HTML doesn't match");
        return;
    }
    printf("  content match: OK\n");
    
    render_free(result);
    printf("  render_free: OK\n");
    
    test_pass();
}

void test_render_escape_html(void) {
    test_start("HTML escaping");
    
    const char *dangerous = "<script>alert('xss')</script>";
    char *escaped = render_escape_html(dangerous);
    
    if (escaped == NULL) {
        test_fail("render_escape_html failed");
        return;
    }
    printf("  Input: %s\n", dangerous);
    printf("  Escaped: %s\n", escaped);
    
    if (strstr(escaped, "<script>") != NULL) {
        free(escaped);
        test_fail("HTML not properly escaped");
        return;
    }
    printf("  No script tags in output: OK\n");
    
    free(escaped);
    
    const char *test_chars = "<>&\"'";
    escaped = render_escape_html(test_chars);
    if (escaped == NULL) {
        test_fail("render_escape_html failed on special chars");
        return;
    }
    printf("  Special chars escaped: %s\n", escaped);
    free(escaped);
    
    test_pass();
}

void test_render_null_input(void) {
    test_start("Render module NULL input handling");
    
    char *escaped = render_escape_html(NULL);
    if (escaped != NULL) {
        free(escaped);
        test_fail("render_escape_html should return NULL for NULL input");
        return;
    }
    printf("  NULL input handled: OK\n");
    
    test_pass();
}

void test_db_module_init_close(void) {
    test_start("Database module init/close");
    
    const char *test_db = "test_modules_compat.db";
    
    int rc = db_init(test_db);
    if (rc != 0) {
        test_fail("db_init failed");
        return;
    }
    printf("  db_init: OK\n");
    
    sqlite3 *conn = db_get_connection();
    if (conn == NULL) {
        db_close();
        test_fail("db_get_connection returned NULL");
        return;
    }
    printf("  db_get_connection: OK\n");
    
    db_close();
    printf("  db_close: OK\n");
    
    unlink(test_db);
    
    test_pass();
}

void test_db_module_exec(void) {
    test_start("Database SQL execution");
    
    const char *test_db = "test_modules_compat.db";
    
    if (db_init(test_db) != 0) {
        test_fail("db_init failed");
        return;
    }
    
    const char *create_sql = "CREATE TABLE test (id INTEGER PRIMARY KEY, name TEXT);";
    if (db_exec(create_sql) != 0) {
        db_close();
        unlink(test_db);
        test_fail("db_exec CREATE TABLE failed");
        return;
    }
    printf("  CREATE TABLE: OK\n");
    
    const char *insert_sql = "INSERT INTO test (name) VALUES ('test1');";
    if (db_exec(insert_sql) != 0) {
        db_close();
        unlink(test_db);
        test_fail("db_exec INSERT failed");
        return;
    }
    printf("  INSERT: OK\n");
    
    sqlite3_stmt *stmt = db_prepare("SELECT * FROM test;");
    if (stmt == NULL) {
        db_close();
        unlink(test_db);
        test_fail("db_prepare failed");
        return;
    }
    printf("  SELECT prepare: OK\n");
    
    int result = db_step(stmt);
    if (result != SQLITE_ROW) {
        db_finalize(stmt);
        db_close();
        unlink(test_db);
        test_fail("db_step failed");
        return;
    }
    printf("  SELECT step: OK\n");
    
    db_finalize(stmt);
    printf("  db_finalize: OK\n");
    
    db_close();
    unlink(test_db);
    
    test_pass();
}

void test_db_module_migrate(void) {
    test_start("Database migration");
    
    const char *test_db = "test_modules_compat.db";
    
    if (db_init(test_db) != 0) {
        test_fail("db_init failed");
        return;
    }
    
    if (db_migrate() != 0) {
        db_close();
        unlink(test_db);
        test_fail("db_migrate failed");
        return;
    }
    printf("  db_migrate: OK\n");
    
    sqlite3_stmt *stmt = db_prepare("SELECT name FROM sqlite_master WHERE type='table';");
    if (stmt == NULL) {
        db_close();
        unlink(test_db);
        test_fail("Failed to query tables");
        return;
    }
    
    int table_count = 0;
    while (db_step(stmt) == SQLITE_ROW) {
        const unsigned char *table_name = sqlite3_column_text(stmt, 0);
        printf("  Found table: %s\n", table_name);
        table_count++;
    }
    
    db_finalize(stmt);
    
    if (table_count < 3) {
        db_close();
        unlink(test_db);
        test_fail("Not all migration tables created");
        return;
    }
    printf("  Migration tables created: %d\n", table_count);
    
    db_close();
    unlink(test_db);
    
    test_pass();
}

void test_http_server_init(void) {
    test_start("HTTP server initialization");
    
    int rc = http_server_init(8080);
    if (rc != 0) {
        test_fail("http_server_init failed");
        return;
    }
    printf("  http_server_init(8080): OK\n");
    
    http_server_shutdown();
    printf("  http_server_shutdown: OK\n");
    
    test_pass();
}

void test_integration_full_stack(void) {
    test_start("Full stack integration");
    
    const char *test_db = "test_modules_compat.db";
    
    if (db_init(test_db) != 0) {
        test_fail("db_init failed");
        return;
    }
    printf("  Database initialized\n");
    
    if (db_migrate() != 0) {
        db_close();
        unlink(test_db);
        test_fail("db_migrate failed");
        return;
    }
    printf("  Database migrated\n");
    
    router_init();
    printf("  Router initialized\n");
    
    router_add_route("GET", "/", test_route_handler);
    router_add_route("GET", "/api/test", test_route_handler);
    printf("  Routes registered\n");
    
    http_request_t req = {
        .method = "GET",
        .path = "/",
        .query_string = NULL,
        .body = NULL,
        .body_len = 0,
        .content_type = NULL
    };
    
    http_response_t *response = router_dispatch(&req);
    if (response == NULL) {
        router_cleanup();
        db_close();
        unlink(test_db);
        test_fail("router_dispatch failed");
        return;
    }
    printf("  Request dispatched\n");
    
    if (response->status_code != 200) {
        http_response_free(response);
        router_cleanup();
        db_close();
        unlink(test_db);
        test_fail("unexpected response");
        return;
    }
    printf("  Response generated: %d\n", response->status_code);
    
    http_response_free(response);
    router_cleanup();
    db_close();
    unlink(test_db);
    
    printf("  Full cleanup completed\n");
    
    test_pass();
}

int main(void) {
    printf("\n");
    printf(ANSI_COLOR_BLUE "======================================\n");
    printf("  Module Compatibility Tests\n");
    printf("======================================" ANSI_COLOR_RESET "\n\n");
    
    test_http_module();
    test_http_response_empty_body();
    test_router_module();
    test_router_not_found();
    test_render_module();
    test_render_escape_html();
    test_render_null_input();
    test_db_module_init_close();
    test_db_module_exec();
    test_db_module_migrate();
    test_http_server_init();
    test_integration_full_stack();
    
    printf(ANSI_COLOR_BLUE "======================================\n");
    printf("  Test Summary\n");
    printf("======================================" ANSI_COLOR_RESET "\n");
    printf("Total tests:  %d\n", test_count);
    printf(ANSI_COLOR_GREEN "Passed:       %d" ANSI_COLOR_RESET "\n", test_passed);
    if (test_failed > 0) {
        printf(ANSI_COLOR_RED "Failed:       %d" ANSI_COLOR_RESET "\n", test_failed);
    } else {
        printf("Failed:       %d\n", test_failed);
    }
    printf(ANSI_COLOR_BLUE "======================================" ANSI_COLOR_RESET "\n\n");
    
    if (test_failed > 0) {
        printf(ANSI_COLOR_RED "❌ Some tests failed!" ANSI_COLOR_RESET "\n");
        return 1;
    }
    
    printf(ANSI_COLOR_GREEN "✅ All module compatibility tests passed!" ANSI_COLOR_RESET "\n");
    return 0;
}
