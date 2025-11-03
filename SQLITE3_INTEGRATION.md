# SQLite3 Integration Guide

This document describes the SQLite3 integration in the Cosmopolitan Web Application.

## Overview

SQLite3 has been integrated as an embedded database, compiled statically into the application binary. This provides a zero-configuration, serverless database solution that works across all supported platforms.

## Components

### 1. SQLite3 Source Code

Located in `third_party/sqlite3/`:
- **sqlite3.c** - Complete SQLite3 implementation (amalgamation)
- **sqlite3.h** - Public API header
- **sqlite3ext.h** - Extension API header
- **Version**: 3.46.1 (August 2024)

### 2. Database Wrapper Layer

Located in `src/`:
- **db.h** - Database API interface
- **db.c** - Wrapper implementation

The wrapper provides simplified functions:
- `db_init(path)` - Initialize database connection
- `db_close()` - Close database connection
- `db_exec(sql)` - Execute SQL statement
- `db_prepare(sql)` - Prepare SQL statement
- `db_step(stmt)` - Execute prepared statement
- `db_finalize(stmt)` - Finalize prepared statement
- `db_migrate()` - Run database migrations

### 3. Test Suite

Located in `tests/`:
- **test_sqlite3.c** - Direct SQLite3 API tests (7 test cases)
- **test_db.c** - Database wrapper tests (6 test cases)

## Build System Integration

### Makefile

The Makefile has been updated to:
- Include SQLite3 header path: `-Ithird_party/sqlite3`
- Compile SQLite3 amalgamation with optimizations
- Link SQLite3 into the main application
- Support test compilation and execution

Build flags for SQLite3:
```makefile
-DSQLITE_THREADSAFE=0          # Single-threaded mode
-DSQLITE_OMIT_LOAD_EXTENSION   # Disable dynamic loading
```

### Build Script

The `build.sh` script has been updated to include SQLite3 compilation.

## Database Schema

The application includes a migration system that creates the following tables:

### boards
- `id` INTEGER PRIMARY KEY AUTOINCREMENT
- `name` TEXT NOT NULL UNIQUE
- `title` TEXT NOT NULL
- `description` TEXT
- `created_at` DATETIME DEFAULT CURRENT_TIMESTAMP

### threads
- `id` INTEGER PRIMARY KEY AUTOINCREMENT
- `board_id` INTEGER NOT NULL (FK → boards.id)
- `subject` TEXT NOT NULL
- `created_at` DATETIME DEFAULT CURRENT_TIMESTAMP

### posts
- `id` INTEGER PRIMARY KEY AUTOINCREMENT
- `thread_id` INTEGER NOT NULL (FK → threads.id)
- `author` TEXT
- `content` TEXT NOT NULL
- `created_at` DATETIME DEFAULT CURRENT_TIMESTAMP

## Usage Examples

### Initialize Database
```c
#include "db.h"

int main(void) {
    if (db_init("app.db") != 0) {
        fprintf(stderr, "Failed to initialize database\n");
        return 1;
    }
    
    // Run migrations
    if (db_migrate() != 0) {
        fprintf(stderr, "Failed to run migrations\n");
        db_close();
        return 1;
    }
    
    // Your application code here
    
    db_close();
    return 0;
}
```

### Execute SQL
```c
int rc = db_exec("INSERT INTO boards (name, title) VALUES ('tech', 'Technology');");
if (rc != 0) {
    fprintf(stderr, "Insert failed\n");
}
```

### Query Data
```c
sqlite3_stmt *stmt = db_prepare("SELECT id, name FROM boards;");
if (stmt != NULL) {
    while (db_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        printf("Board %d: %s\n", id, name);
    }
    db_finalize(stmt);
}
```

### Prepared Statements with Parameters
```c
sqlite3_stmt *stmt = db_prepare(
    "INSERT INTO posts (thread_id, author, content) VALUES (?, ?, ?);"
);
if (stmt != NULL) {
    sqlite3_bind_int(stmt, 1, thread_id);
    sqlite3_bind_text(stmt, 2, author, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, content, -1, SQLITE_STATIC);
    
    if (db_step(stmt) == SQLITE_DONE) {
        printf("Post created successfully\n");
    }
    db_finalize(stmt);
}
```

## Testing

### Run All Tests
```bash
make test
```

### Run Tests in Development Mode
```bash
BUILD_MODE=gcc make test
```

### Run Individual Test
```bash
./obj/test_sqlite3
./obj/test_db
```

### Expected Output
```
======================================
  SQLite3 Integration Test Suite
======================================

[TEST 1] SQLite3 version check
  SQLite version: 3.46.1
  ✓ PASSED

... (more tests)

======================================
  Test Summary
======================================
Total tests:  13
Passed:       13
Failed:       0
======================================
All tests passed!
```

## Build Modes

### Cosmopolitan Build (Default)
```bash
make
```
Creates `app.com` - Actually Portable Executable

### GCC Build (Development)
```bash
make dev
# or
BUILD_MODE=gcc make
```
Creates `app` - Standard Linux executable

## Platform Support

SQLite3 works on all platforms supported by Cosmopolitan Libc:
- Linux (x86_64, ARM64)
- Windows (x86_64)
- macOS (x86_64, ARM64)
- FreeBSD, OpenBSD, NetBSD

## Performance Considerations

1. **Single-threaded mode** (`SQLITE_THREADSAFE=0`) provides better performance
2. **Static linking** eliminates runtime dependencies
3. **Embedded database** means no network latency
4. **In-process** execution for maximum efficiency

## Security

1. **Extension loading disabled** - Prevents dynamic code loading
2. **Prepared statements** - Protects against SQL injection
3. **No external dependencies** - Reduces attack surface
4. **Static compilation** - Tamper-resistant binary

## Debugging

### Enable SQLite3 Debug Output
Modify compile flags in Makefile:
```makefile
-DSQLITE_DEBUG
```

### View SQL Execution
The wrapper functions print SQL statements and errors to stderr.

### Inspect Database
```bash
# Using SQLite3 CLI (if installed)
sqlite3 app.db

# List tables
.tables

# View schema
.schema

# Query data
SELECT * FROM boards;
```

## Troubleshooting

### Build Errors

**Problem**: `sqlite3.h: No such file or directory`
**Solution**: Ensure `third_party/sqlite3/` directory exists with SQLite3 files

**Problem**: Linker errors with SQLite3 symbols
**Solution**: Verify `$(SQLITE3_OBJ)` is included in link command

### Runtime Errors

**Problem**: `Database not initialized`
**Solution**: Call `db_init()` before using database functions

**Problem**: SQL syntax errors
**Solution**: Check SQL syntax, use prepared statements for dynamic queries

## Future Enhancements

Potential improvements:
- Connection pooling (if multi-threading is enabled)
- Query result caching
- Database backup/restore functionality
- Migration version tracking
- Performance monitoring
- Full-text search integration

## References

- [SQLite Official Documentation](https://www.sqlite.org/docs.html)
- [SQLite C/C++ API](https://www.sqlite.org/c3ref/intro.html)
- [Cosmopolitan Libc](https://github.com/jart/cosmopolitan)
- [Project README](README.md)

## License

SQLite3 is in the public domain. See https://www.sqlite.org/copyright.html
