# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

### Added
- SQLite3 integration (version 3.46.1)
  - Added SQLite3 amalgamation source in `third_party/sqlite3/`
  - Implemented database wrapper layer in `src/db.c` and `src/db.h`
  - Added comprehensive test suite with 13 test cases
    - 7 tests for SQLite3 API (`tests/test_sqlite3.c`)
    - 6 tests for database wrapper (`tests/test_db.c`)
  - Updated build system (Makefile and build.sh) to compile and link SQLite3
  - Added database migrations for boards, threads, and posts tables
  - Created `SQLITE3_INTEGRATION.md` documentation

### Changed
- Updated `Makefile` to:
  - Include SQLite3 header path
  - Compile SQLite3 with appropriate flags
  - Add test compilation and execution targets
  - Clean up test database files
- Updated `build.sh` to compile SQLite3 amalgamation
- Enhanced `db.c` from stub implementation to full SQLite3 wrapper
- Updated `.gitignore` to exclude database files

### Build Configuration
- SQLite3 compiled with:
  - `SQLITE_THREADSAFE=0` - Single-threaded mode for performance
  - `SQLITE_OMIT_LOAD_EXTENSION` - Security hardening
- Added linker flags: `-lpthread -ldl -lm` (for GCC builds)

### Testing
- All 13 tests pass successfully
- Test coverage includes:
  - Database connection lifecycle
  - Table creation and schema management
  - Data insertion and queries
  - Prepared statements with parameters
  - Transaction support
  - Error handling
  - Migration system

## [0.1.0] - Initial Release

### Added
- Basic project structure
- Module stubs for HTTP, routing, database, rendering, admin, board, and upload
- Makefile with Cosmopolitan and GCC build support
- Documentation (README, ARCHITECTURE, CONTRIBUTING, etc.)
