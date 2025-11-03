# SQLite3 Integration

This directory contains the SQLite3 amalgamation for embedded database functionality.

## Version

SQLite 3.46.1 (2024-08-13)

## Files

- `sqlite3.c` - SQLite3 amalgamation source code
- `sqlite3.h` - SQLite3 public API header
- `sqlite3ext.h` - SQLite3 extension header
- `shell.c` - SQLite3 command-line shell (not used in build)

## Build Configuration

The SQLite3 library is compiled with the following flags:

- `SQLITE_THREADSAFE=0` - Disable threading support for single-threaded usage
- `SQLITE_OMIT_LOAD_EXTENSION` - Disable dynamic extension loading for security

## Integration

SQLite3 is statically linked into the application binary. The `src/db.c` module provides a wrapper API around SQLite3 functions.

## License

SQLite3 is in the public domain. See https://www.sqlite.org/copyright.html for details.

## Source

Downloaded from: https://www.sqlite.org/download.html

For the latest version, visit: https://www.sqlite.org/

## Testing

SQLite3 integration is tested via:

- `tests/test_sqlite3.c` - Direct SQLite3 API tests
- `tests/test_db.c` - Database wrapper layer tests

Run tests with:
```bash
make test
```
