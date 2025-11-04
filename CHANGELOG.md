# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

### Added
- **Internationalization (i18n)**
  - Multi-language support (English, Simplified Chinese)
  - Language detection from URL parameter, cookie, or default
  - Language switcher UI in all pages
  - Cookie-based language preference (1 year expiry)
  - Comprehensive translation system with 50+ translation keys
  
- **Material Design UI**
  - Complete UI redesign following Material Design principles
  - Gradient headers with custom color schemes
  - Card-based layouts with elevation effects
  - Responsive design with mobile/desktop breakpoints
  - Touch-friendly buttons (48px min-height)
  - Common CSS module for consistent styling
  
- **Kaomoji Emoticon Picker**
  - 12 categories with 100+ Japanese emoticons
  - Interactive picker UI in message forms
  - Categories: Happy, Excited, Love, Sad, Angry, Confused, Surprised, Cute, Animals, Actions, Objects, Symbols
  - One-click insertion into text fields
  
- **Code Refactoring**
  - Extracted `auth.c/h` for authentication and session management
  - Extracted `utils.c/h` for common utility functions
  - Extracted `kaomoji.c/h` for emoticon data
  - Extracted `html_template.c/h` for HTML templates and common CSS
  - Reduced code duplication and improved modularity
  - Better separation of concerns

### Fixed
- **Thread View SIGSEGV Crash**
  - Fixed incorrect parameter order in `snprintf()` causing segmentation fault
  - Thread pages now load correctly without crashes
  - Added test suite to prevent regression
  
- **Database ENOENT/Segfault Issues**
  - Added directory existence checks before database initialization
  - Disabled SQLite memory-mapped I/O (`PRAGMA mmap_size=0`) to prevent SIGSEGV on file deletion
  - Enabled WAL mode (`PRAGMA journal_mode=WAL`) for better concurrency
  - Set synchronous mode to NORMAL for improved performance and safety
  - Added 5-second busy timeout to handle lock contention
  - Auto-create upload directory if it doesn't exist
  
### Changed
- **CI/CD Enhancement**
  - Added automatic GitHub Release creation when new tags are pushed
  - Release includes compiled binary (`app.com`) and SHA256 checksums
  - Release notes are automatically generated from commits
  - Fixed release permissions using `secrets.GITHUB_TOKEN`

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
