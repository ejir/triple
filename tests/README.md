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
