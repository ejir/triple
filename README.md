# Cosmopolitan Web Application

A portable web application built with [Cosmopolitan Libc](https://github.com/jart/cosmopolitan) that compiles to an Actually Portable Executable (APE) - a single binary that runs natively on Linux, Windows, MacOS, FreeBSD, OpenBSD, and NetBSD.

English | [简体中文](README.zh-CN.md)

## Features

- **Actually Portable**: Single binary runs on multiple operating systems
- **Embedded SQLite**: Static database with no external dependencies
- **Modular Architecture**: Clean separation of concerns with dedicated modules
- **HTTP Server**: Standalone web server with routing capabilities
- **Admin Interface**: Built-in administration panel with session management
- **File Uploads**: Support for file upload functionality
- **Message Board**: Forum/imageboard-style functionality with threads and posts
- **Internationalization**: Multi-language support (English, 简体中文) with easy language switching
- **Material Design UI**: Modern, responsive interface following Material Design principles
- **Kaomoji Picker**: Built-in Japanese emoticon picker for enhanced messaging

## Directory Structure

```
.
├── src/                    # Source code directory
│   ├── main.c             # Application entry point and initialization
│   ├── http.c/h           # HTTP server implementation
│   ├── router.c/h         # URL routing and request dispatching
│   ├── db.c/h             # Database abstraction layer (SQLite)
│   ├── render.c/h         # HTML rendering and templating
│   ├── admin.c/h          # Admin panel handlers
│   ├── auth.c/h           # Authentication and session management
│   ├── board.c/h          # Message board functionality
│   ├── thread.h           # Thread handlers (placeholder)
│   ├── post.h             # Post handlers (placeholder)
│   ├── upload.c/h         # File upload handling
│   ├── i18n.c/h           # Internationalization/localization
│   ├── kaomoji.c/h        # Kaomoji emoticon data and picker
│   ├── utils.c/h          # Common utility functions
│   └── html_template.c/h  # HTML template helpers and common CSS
├── third_party/           # Third-party libraries
│   └── sqlite3/           # SQLite3 amalgamation
├── tests/                 # Test suite
├── obj/                   # Compiled object files (generated)
├── doc/                   # Documentation directory
├── Makefile              # Primary build system
├── build.sh              # Alternative build script
└── README.md             # This file
```

## Prerequisites

You need the Cosmopolitan toolchain installed on your system.

### Installing Cosmopolitan

**Option 1: Download pre-built binaries**

```bash
# Download and extract to /opt/cosmo
sudo mkdir -p /opt/cosmo
cd /opt/cosmo
sudo wget https://cosmo.zip/pub/cosmocc/cosmocc.zip
sudo unzip cosmocc.zip
sudo rm cosmocc.zip
sudo chmod +x bin/cosmocc bin/cosmoar
```

**Option 2: Build from source**

```bash
git clone https://github.com/jart/cosmopolitan.git
cd cosmopolitan
make -j$(nproc)
sudo make install PREFIX=/opt/cosmo
```

**Option 3: Use custom location**

If you install Cosmopolitan to a different location, set the `COSMO_DIR` environment variable:

```bash
export COSMO_DIR=/path/to/your/cosmo
```

## Building

### Using Make (Recommended)

```bash
make
```

This will compile all source files and produce `app.com`, an Actually Portable Executable.

**Additional make targets:**

- `make clean` - Remove build artifacts
- `make run` - Build and run the application
- `make help` - Display help information

### Using build.sh

```bash
./build.sh
```

The build script provides an alternative way to build the project without Make.

### Build Output

After a successful build, you will have:

- `app.com` - The Actually Portable Executable
- `obj/*.o` - Compiled object files

## Running

Simply execute the binary:

```bash
./app.com
```

The application will:
1. Initialize the database
2. Run migrations
3. Set up routing
4. Start the HTTP server on port 8080

Access the application at: `http://localhost:8080`

**Keyboard shortcuts:**
- `Ctrl+C` - Gracefully shutdown the server

## Module Documentation

### HTTP Module (`http.c/h`)

Handles HTTP protocol implementation, server lifecycle, and response generation.

**Key Functions:**
- `http_server_init()` - Initialize server on specified port
- `http_server_run()` - Main server loop
- `http_response_create()` - Create HTTP responses
- `http_response_free()` - Clean up response objects

### Router Module (`router.c/h`)

URL routing and request dispatching to appropriate handlers.

**Key Functions:**
- `router_init()` - Initialize routing table
- `router_add_route()` - Register route handlers
- `router_dispatch()` - Match requests to handlers

### Database Module (`db.c/h`)

SQLite database abstraction with connection management and query helpers.

**Key Functions:**
- `db_init()` - Open database connection
- `db_migrate()` - Run schema migrations
- `db_prepare()` - Prepare SQL statements
- `db_exec()` - Execute SQL queries

### Render Module (`render.c/h`)

HTML generation and templating system.

**Key Functions:**
- `render_template()` - Render templates with data
- `render_html()` - Create HTML responses
- `render_escape_html()` - Sanitize user input

### Board Module (`board.c/h`)

Message board/forum functionality with threads and posts.

**Key Functions:**
- `board_list_handler()` - List all boards
- `board_view_handler()` - View board threads
- `thread_view_handler()` - View thread posts
- `thread_create_handler()` - Create new threads
- `post_create_handler()` - Add posts to threads

### Admin Module (`admin.c/h`)

Administrative interface for managing the application.

**Key Functions:**
- `admin_dashboard_handler()` - Admin control panel
- `admin_login_handler()` - Authentication
- `admin_logout_handler()` - Session termination
- `admin_board_create_handler()` - Create new boards
- `admin_board_delete_handler()` - Delete boards

### Auth Module (`auth.c/h`)

Centralized authentication and session management.

**Key Functions:**
- `auth_is_authenticated()` - Check if user has valid session
- `auth_create_session()` - Create new session token
- `auth_destroy_session()` - Invalidate session

### Upload Module (`upload.c/h`)

File upload handling and storage.

**Key Functions:**
- `upload_handler()` - Handle file uploads
- `upload_parse_multipart()` - Parse multipart form data
- `upload_save_file()` - Save uploaded files

### Internationalization Module (`i18n.c/h`)

Multi-language support with translation management and language detection.

**Key Functions:**
- `i18n_get_language()` - Detect user's language preference (from URL, cookie, or default)
- `i18n_get()` - Get translated text for a key
- `i18n_get_lang_code()` - Get language code (e.g., "en", "zh-cn")
- `i18n_get_lang_name()` - Get language display name

**Supported Languages:**
- English (`LANG_EN`)
- Simplified Chinese (`LANG_ZH_CN`)

For detailed localization documentation, see [doc/LOCALIZATION.md](doc/LOCALIZATION.md).

### Kaomoji Module (`kaomoji.c/h`)

Japanese emoticon (kaomoji) data and picker functionality.

**Key Functions:**
- `kaomoji_get_categories()` - Get all emoticon categories
- `kaomoji_get_categories_count()` - Get category count

**Categories:**
- Happy, Excited, Love, Sad, Angry, Confused, Surprised, Cute, Animals, Actions, Objects, Symbols

### Utils Module (`utils.c/h`)

Common utility functions used across the application.

**Key Functions:**
- `url_decode()` - URL decoding for query strings
- `get_cookie_value()` - Parse cookie values from headers
- `generate_random_token()` - Generate secure random tokens

### HTML Template Module (`html_template.c/h`)

Common HTML rendering functions and shared CSS.

**Key Functions:**
- `html_get_common_css()` - Returns Material Design CSS
- `html_render_header()` - Renders HTML document header
- `html_render_footer()` - Renders HTML document footer

## Development

### Code Style

- **Indentation**: 4 spaces (no tabs)
- **Line Length**: Maximum 100 characters
- **Naming Conventions**:
  - Functions: `snake_case`
  - Structs: `snake_case_t` suffix
  - Constants: `UPPER_SNAKE_CASE`
  - Macros: `UPPER_SNAKE_CASE`
- **Braces**: K&R style (opening brace on same line)
- **Comments**: Use `//` for single-line, `/* */` for multi-line

### Adding New Modules

1. Create `src/module.h` with function declarations and type definitions
2. Create `src/module.c` with implementations
3. Add `#include "module.h"` to `main.c`
4. Call initialization function from `main()`
5. Register routes if needed
6. Add module to `SOURCES` in `build.sh` (Makefile will auto-detect)

### Testing

After building, test the application:

```bash
./app.com
```

Verify that:
- The application starts without errors
- All modules initialize successfully
- HTTP server starts on the configured port
- Routes are registered properly

## Contributing

### Workflow

1. Create a feature branch
2. Make your changes
3. Test thoroughly
4. Submit a pull request

### Code Guidelines

- Keep functions focused and small (< 50 lines when possible)
- Add error handling for all system calls
- Use `const` for immutable parameters
- Free all allocated memory
- Check all return values
- Document complex algorithms

### Commit Messages

Use conventional commit format:

```
<type>(<scope>): <subject>

<body>

<footer>
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

Example:
```
feat(router): add wildcard route matching

Implement support for wildcard patterns in route definitions
to enable more flexible URL routing.

Closes #123
```

## Troubleshooting

### Cosmopolitan not found

**Error**: `Error: Cosmopolitan toolchain not found`

**Solution**: Install Cosmopolitan or set `COSMO_DIR`:
```bash
export COSMO_DIR=/path/to/cosmopolitan
```

### Permission denied when running

**Error**: `bash: ./app.com: Permission denied`

**Solution**: Make the binary executable:
```bash
chmod +x app.com
```

### Port already in use

**Error**: `Failed to initialize HTTP server`

**Solution**: Change the port or stop the conflicting service:
```bash
# Find process using port 8080
lsof -i :8080
# Kill the process or change DEFAULT_PORT in main.c
```

## Architecture

### Request Flow

```
HTTP Request
    ↓
http_server_run() - Accept connection
    ↓
router_dispatch() - Match route
    ↓
Route Handler - Process request
    ↓
render_template() - Generate HTML
    ↓
http_response_create() - Build response
    ↓
HTTP Response
```

### Data Flow

```
Client → HTTP Server → Router → Handler → Database
                                   ↓
Client ← HTTP Server ← Router ← Response ← Render
```

## Documentation

📚 **See [doc/DOCS_INDEX.md](doc/DOCS_INDEX.md) for a complete documentation guide**

### Core Documentation
- [README.md](README.md) - This file (project overview)
- [README.zh-CN.md](README.zh-CN.md) - Chinese version of README
- [doc/ARCHITECTURE.md](doc/ARCHITECTURE.md) - System architecture and design
- [doc/INSTALL.md](doc/INSTALL.md) - Installation and build instructions
- [doc/CONTRIBUTING.md](doc/CONTRIBUTING.md) - Contribution guidelines

### Feature Documentation
- [doc/LOCALIZATION.md](doc/LOCALIZATION.md) - Internationalization/localization guide
- [doc/UI_FEATURES.md](doc/UI_FEATURES.md) - Material Design UI and Kaomoji picker
- [doc/DATABASE_SCHEMA.md](doc/DATABASE_SCHEMA.md) - Database schema and queries
- [doc/API.md](doc/API.md) - HTTP endpoints and API reference

### Development History
- [doc/CHANGELOG.md](doc/CHANGELOG.md) - Version history and changes
- [doc/REFACTORING_SUMMARY.md](doc/REFACTORING_SUMMARY.md) - Code refactoring details
- [doc/BUGFIX_SUMMARY.md](doc/BUGFIX_SUMMARY.md) - Database stability fixes
- [doc/BUGFIX_THREAD_CRASH.md](doc/BUGFIX_THREAD_CRASH.md) - Thread view crash fix

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [Cosmopolitan Libc](https://github.com/jart/cosmopolitan) by Justine Tunney
- [SQLite](https://www.sqlite.org/) for the embedded database
- [Material Design](https://material.io/) for UI/UX principles
