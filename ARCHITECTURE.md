# Architecture Documentation

This document describes the architecture and design of the Cosmopolitan web application.

## Overview

The application is designed as a modular, portable web server with database capabilities. It compiles to an Actually Portable Executable (APE) that runs on multiple operating systems without modification.

## Technology Stack

- **Language**: C11
- **Libc**: Cosmopolitan Libc (portable across Linux, Windows, macOS, BSD)
- **Database**: SQLite (statically linked)
- **Build System**: Make + shell scripts
- **HTTP**: Custom implementation (stub, ready for expansion)

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                         main.c                              │
│              (Application Entry Point)                      │
└────────────────────┬────────────────────────────────────────┘
                     │
                     │ Initializes and coordinates
                     ↓
         ┌───────────────────────────┐
         │   HTTP Server (http.c)    │
         │  - Accept connections     │
         │  - Parse requests         │
         │  - Send responses         │
         └──────────┬────────────────┘
                    │
                    │ Routes requests to
                    ↓
         ┌───────────────────────────┐
         │   Router (router.c)       │
         │  - URL pattern matching   │
         │  - Handler dispatch       │
         └──────────┬────────────────┘
                    │
                    │ Dispatches to
                    ↓
    ┌───────────────┴───────────────┬──────────────┐
    │                               │              │
    ↓                               ↓              ↓
┌──────────┐              ┌──────────────┐  ┌──────────┐
│ Board    │              │    Admin     │  │  Upload  │
│ (board.c)│              │  (admin.c)   │  │(upload.c)│
│- Threads │              │- Dashboard   │  │- Files   │
│- Posts   │              │- Auth        │  │- Storage │
└─────┬────┘              └──────┬───────┘  └─────┬────┘
      │                          │                 │
      │ Uses                     │ Uses            │ Uses
      ↓                          ↓                 ↓
┌──────────────────────────────────────────────────────────┐
│                    Database (db.c)                       │
│              - SQLite connection management              │
│              - Query execution                           │
│              - Migrations                                │
└─────────────────────────┬────────────────────────────────┘
                          │
                          ↓
                    ┌───────────┐
                    │  SQLite   │
                    │ Database  │
                    └───────────┘

                    ┌──────────────┐
          ┌─────────┤   Render     │
          │         │  (render.c)  │
          │         │- Templating  │
          │         │- HTML gen    │
          │         └──────────────┘
          │
          └─── Used by all handlers for output
```

## Module Descriptions

### main.c - Application Entry Point

**Responsibility**: Application lifecycle management

**Key Functions**:
- `main()` - Entry point, initialization, event loop

**Responsibilities**:
- Parse command-line arguments
- Initialize all modules in correct order
- Set up signal handlers (SIGINT, SIGTERM)
- Start HTTP server
- Clean shutdown on exit

**Dependencies**: All modules

### http.c/h - HTTP Server Module

**Responsibility**: HTTP protocol implementation

**Key Structures**:
```c
typedef struct {
    const char *method;
    const char *path;
    const char *query_string;
    const char *body;
    size_t body_len;
    const char *content_type;
} http_request_t;

typedef struct {
    int status_code;
    const char *content_type;
    char *body;
    size_t body_len;
} http_response_t;
```

**Key Functions**:
- `http_server_init()` - Initialize server socket
- `http_server_run()` - Main event loop
- `http_server_shutdown()` - Clean shutdown
- `http_response_create()` - Build responses
- `http_response_free()` - Clean up responses

**Status**: Stub implementation, ready for full HTTP server

### router.c/h - URL Router Module

**Responsibility**: Request routing and handler dispatch

**Key Structures**:
```c
typedef http_response_t *(*route_handler_t)(http_request_t *req);

typedef struct {
    const char *method;
    const char *path;
    route_handler_t handler;
} route_t;
```

**Key Functions**:
- `router_init()` - Initialize routing table
- `router_add_route()` - Register route handlers
- `router_dispatch()` - Match and dispatch requests
- `router_cleanup()` - Clean up resources

**Features**:
- Method-based routing (GET, POST, etc.)
- Exact path matching (ready to extend with patterns)
- 404 handling for unmatched routes

### db.c/h - Database Module

**Responsibility**: Database abstraction layer

**Key Types**:
```c
typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;
```

**Key Functions**:
- `db_init()` - Open database connection
- `db_close()` - Close connection
- `db_exec()` - Execute SQL
- `db_prepare()` - Prepare statements
- `db_step()` - Iterate results
- `db_finalize()` - Clean up statements
- `db_migrate()` - Run schema migrations

**Features**:
- Connection pooling (ready to implement)
- Prepared statements
- Transaction support (ready to implement)
- Migration system (stub)

### render.c/h - Rendering Module

**Responsibility**: HTML generation and templating

**Key Structures**:
```c
typedef struct {
    char *html;
    size_t len;
} render_result_t;
```

**Key Functions**:
- `render_template()` - Render templates with data
- `render_html()` - Create HTML responses
- `render_escape_html()` - Sanitize user input
- `render_free()` - Clean up rendered output

**Features**:
- HTML escaping for security
- Template system (stub, ready for implementation)
- Memory-safe string handling

### board.c/h - Board/Forum Module

**Responsibility**: Message board functionality

**Key Structures**:
```c
typedef struct {
    int64_t id;
    char *name;
    char *description;
} board_t;

typedef struct {
    int64_t id;
    int64_t board_id;
    char *subject;
    char *content;
    char *author;
    int64_t created_at;
} thread_t;

typedef struct {
    int64_t id;
    int64_t thread_id;
    char *content;
    char *author;
    int64_t created_at;
} post_t;
```

**Key Functions**:
- `board_list_handler()` - List all boards
- `board_view_handler()` - View board with threads
- `thread_view_handler()` - View thread with posts
- `thread_create_handler()` - Create new thread
- `post_create_handler()` - Add post to thread
- `board_get_by_id()` - Fetch board data
- `thread_get_by_id()` - Fetch thread data

**Routes**:
- `GET /` - Board list
- `GET /board?id=<id>` - View board
- `GET /thread?id=<id>` - View thread
- `POST /thread` - Create thread
- `POST /post` - Create post

### admin.c/h - Administration Module

**Responsibility**: Admin panel and management

**Key Functions**:
- `admin_dashboard_handler()` - Admin control panel
- `admin_login_handler()` - Authentication
- `admin_logout_handler()` - Session termination

**Routes**:
- `GET /admin` - Dashboard
- `GET /admin/login` - Login form
- `POST /admin/login` - Login submission
- `GET /admin/logout` - Logout

**Features** (to implement):
- User authentication
- Session management
- Administrative controls
- User management
- Content moderation

### upload.c/h - File Upload Module

**Responsibility**: File upload and storage

**Key Structures**:
```c
typedef struct {
    char *filename;
    char *content_type;
    uint8_t *data;
    size_t size;
} upload_file_t;
```

**Key Functions**:
- `upload_handler()` - Handle file uploads
- `upload_parse_multipart()` - Parse multipart/form-data
- `upload_save_file()` - Save file to disk
- `upload_file_free()` - Clean up file structure

**Routes**:
- `POST /upload` - File upload endpoint

**Features** (to implement):
- Multipart form data parsing
- File type validation
- Size limits
- Secure file storage
- Virus scanning (optional)

## Data Flow

### Request Processing Flow

1. **HTTP Server** receives connection
2. **HTTP Server** parses request into `http_request_t`
3. **Router** matches request to handler
4. **Handler** (board/admin/upload) processes request:
   - May query **Database** for data
   - May use **Render** to generate HTML
5. **Handler** returns `http_response_t`
6. **HTTP Server** sends response to client

### Example: Viewing a Thread

```
User Request: GET /thread?id=42
       ↓
http_server_run() receives request
       ↓
router_dispatch() matches to thread_view_handler()
       ↓
thread_view_handler():
  - thread_get_by_id(42) → queries database
  - Gets posts for thread
  - render_template("thread.html", data)
  - Returns http_response_t
       ↓
http_server sends HTML response
       ↓
User sees thread page
```

## Design Patterns

### Module Pattern

Each module has:
- Header file (`.h`) - Public interface
- Source file (`.c`) - Implementation
- Init function - Setup and configuration
- Cleanup function - Resource deallocation

### Resource Management

- **Ownership**: Clear ownership of allocated memory
- **Cleanup**: Every `malloc` has corresponding `free`
- **NULL checks**: All allocations checked
- **RAII-style**: Init/cleanup pairs

### Error Handling

- Return codes: 0 for success, -1 or specific codes for errors
- NULL for pointer failures
- Error messages to stderr
- Resource cleanup on error paths

## Extensibility

### Adding a New Route

```c
// In your_module.c
http_response_t *your_handler(http_request_t *req) {
    // Your implementation
}

// In your_module_register_routes()
void your_module_register_routes(void) {
    router_add_route("GET", "/your-path", your_handler);
}

// In main.c
your_module_init();
your_module_register_routes();
```

### Adding a New Module

1. Create `module.h` and `module.c`
2. Define public interface in `.h`
3. Implement in `.c`
4. Add `module_init()` function
5. Add to build system (Makefile auto-detects `.c` files)
6. Call init from `main()`

## Security Considerations

### Input Validation

- All user input must be validated
- Use `render_escape_html()` for HTML output
- Parameterized queries for database (prevent SQL injection)
- Path traversal prevention in file operations

### Authentication

- Admin panel requires authentication (to implement)
- Session management with secure tokens
- Password hashing (bcrypt/argon2)

### File Uploads

- Validate file types
- Check file sizes
- Store in secure location
- Generate safe filenames

## Performance Considerations

### Database

- Use prepared statements (reusable)
- Add indexes for frequently queried columns
- Consider connection pooling for high load
- Batch operations when possible

### Memory

- Free all allocated memory
- Avoid memory leaks (test with valgrind)
- Reuse buffers when appropriate
- Consider memory pools for frequent allocations

### Concurrency

- Current implementation is single-threaded
- Ready to add multi-threading:
  - Thread pool for request handling
  - Mutex protection for shared resources
  - Per-thread database connections

## Testing Strategy

### Unit Tests

- Test each module independently
- Mock dependencies
- Test error conditions
- Test edge cases

### Integration Tests

- Test module interactions
- Test complete request flows
- Test database operations
- Test file operations

### Manual Tests

- Build and run application
- Test all routes
- Test error handling
- Test with various inputs

## Future Enhancements

### Short Term

- Complete HTTP server implementation
- Add SQLite integration
- Implement basic templating
- Add session management

### Medium Term

- WebSocket support
- TLS/HTTPS support
- Caching layer
- API endpoints (REST/JSON)

### Long Term

- Clustering support
- Load balancing
- Metrics and monitoring
- Admin dashboard features

## Build System

### Makefile Targets

- `make` - Build with Cosmopolitan (APE)
- `make dev` - Build with GCC (fast iteration)
- `make clean` - Remove build artifacts
- `make run` - Build and run
- `make help` - Show help

### Build Modes

- **cosmo** (default): Portable APE binary
- **gcc**: Standard Linux binary for development

### Compiler Flags

- `-O2` - Optimization level 2
- `-Wall -Wextra` - All warnings
- `-std=c11` - C11 standard
- `-Isrc` - Include src directory
- `-static` - Static linking (cosmo mode)

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for:
- Code style guidelines
- Commit message format
- Pull request process
- Testing requirements
