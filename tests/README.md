# Test Suite

This directory contains test files for verifying SQLite3 integration and database functionality.

## Test Files

### test_sqlite3.c

Tests the core SQLite3 functionality directly using the SQLite3 API.

**Test Cases:**
1. **Version Check** - Verifies SQLite3 library version
2. **Open/Close Database** - Tests database connection lifecycle
3. **Create Table** - Tests table creation with DDL
4. **Insert Data** - Tests data insertion and row count
5. **Query Data** - Tests SELECT queries and result iteration
6. **Prepared Statements** - Tests parameterized queries
7. **Transactions** - Tests BEGIN/COMMIT/ROLLBACK operations

### test_db.c

Tests the database wrapper layer (`src/db.c` and `src/db.h`) which provides a simplified API.

**Test Cases:**
1. **Init/Close** - Tests wrapper initialization and cleanup
2. **Exec** - Tests simple SQL execution via wrapper
3. **Prepare/Step** - Tests statement preparation and iteration
4. **Migrate** - Tests database schema migrations
5. **Full Workflow** - Tests complete application workflow (boards, threads, posts)
6. **Error Handling** - Tests error scenarios and edge cases

### test_cosmopolitan_compat.c

Tests Cosmopolitan Libc compatibility and standard C library functions.

**Test Cases:**
1. **Standard Headers** - Verifies all standard C headers are available
2. **Memory Operations** - Tests malloc, free, realloc, calloc, memset
3. **String Operations** - Tests strlen, strcpy, strcat, strstr, strchr
4. **Integer Types** - Verifies stdint.h types (int8_t, uint64_t, etc.)
5. **Time Functions** - Tests time(), gmtime(), localtime(), strftime()
6. **Signal Handling** - Tests signal() and raise()
7. **File Operations** - Tests fopen, fwrite, fread, fclose, unlink
8. **Error Handling** - Tests errno and strerror()
9. **Environment Variables** - Tests getenv()
10. **Math Operations** - Tests arithmetic and 64-bit operations
11. **Pointer Operations** - Tests pointer arithmetic and NULL checks
12. **Struct Operations** - Tests structure manipulation
13. **Platform Info** - Reports platform and compiler information
14. **Buffer Safety** - Tests safe buffer operations

### test_modules_compat.c

Tests all application modules for Cosmopolitan compatibility.

**Test Cases:**
1. **HTTP Module** - Tests response creation and memory management
2. **HTTP Empty Body** - Tests edge case of empty response bodies
3. **Router Module** - Tests route registration and dispatching
4. **Router 404** - Tests 404 not found handling
5. **Render Module** - Tests HTML rendering
6. **HTML Escaping** - Tests XSS prevention via HTML entity escaping
7. **Render NULL Input** - Tests NULL pointer handling
8. **Database Init/Close** - Tests database lifecycle
9. **Database Exec** - Tests SQL execution through db module
10. **Database Migrate** - Tests schema migration
11. **HTTP Server Init** - Tests server initialization
12. **Full Stack Integration** - Tests all modules working together

### test_ape_features.c

Tests Actually Portable Executable (APE) specific features and cross-platform compatibility.

**Test Cases:**
1. **APE Runtime Detection** - Detects if running as APE with Cosmopolitan
2. **Portable Path Handling** - Tests cross-platform path operations
3. **Portable File Creation** - Tests file I/O with POSIX APIs
4. **Portable stdio** - Tests standard I/O operations
5. **Cross-platform Constants** - Verifies O_RDONLY, O_WRONLY, etc.
6. **Memory Alignment** - Tests malloc alignment guarantees
7. **Command Line Args** - Tests argc/argv handling
8. **Portable Types** - Verifies type sizes across platforms
9. **Exit Codes** - Tests EXIT_SUCCESS and EXIT_FAILURE
10. **String Literals** - Tests string literal handling

## Running Tests

### Run all tests:
```bash
make test
```

### Run tests in development mode (GCC):
```bash
BUILD_MODE=gcc make test
```

### Run individual test:
```bash
./obj/test_sqlite3
./obj/test_db
```

## Test Output

Tests use colored output:
- 🟡 Yellow: Test name/header
- 🟢 Green: Test passed
- 🔴 Red: Test failed

Example output:
```
======================================
  SQLite3 Integration Test Suite
======================================

[TEST 1] SQLite3 version check
  SQLite version: 3.46.1
  ✓ PASSED

...

======================================
  Test Summary
======================================
Total tests:  7
Passed:       7
Failed:       0
======================================
```

## Test Database Files

Tests create temporary database files:
- `test_sqlite3.db` - Used by test_sqlite3
- `test_db_wrapper.db` - Used by test_db

These files are automatically cleaned up after each test.

## Adding New Tests

To add a new test file:

1. Create `tests/test_name.c`
2. Add test build rule to `Makefile`:
   ```makefile
   $(OBJ_DIR)/test_name: $(TEST_DIR)/test_name.c $(SQLITE3_OBJ) | $(OBJ_DIR)
       $(CC) $(CFLAGS) $< $(SQLITE3_OBJ) $(LDFLAGS) -o $@
   ```
3. Include necessary dependencies (db.o if testing wrapper)
4. Follow the existing test structure with colored output

## Test Best Practices

- Clean up test databases after each test case
- Use descriptive test names
- Test both success and error paths
- Verify return codes and results
- Print informative messages during test execution
- Use assertions for critical checks
