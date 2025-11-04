# Database Schema Documentation

## Overview

The application uses SQLite3 (version 3.46.1) as its embedded database. The database is configured with Write-Ahead Logging (WAL) mode for better concurrency and has memory-mapped I/O disabled for stability.

## Configuration

### SQLite PRAGMAs

```sql
PRAGMA journal_mode = WAL;        -- Write-Ahead Logging mode
PRAGMA synchronous = NORMAL;      -- Balance between safety and performance
PRAGMA mmap_size = 0;             -- Disable memory-mapped I/O (stability)
```

### Connection Settings

- **Busy Timeout**: 5000ms (5 seconds)
- **Threading Mode**: Single-threaded (`SQLITE_THREADSAFE=0`)
- **Security**: Extensions disabled (`SQLITE_OMIT_LOAD_EXTENSION`)

## Schema

### Tables

#### 1. boards

Stores message board/forum definitions.

```sql
CREATE TABLE boards (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    description TEXT,
    created_at INTEGER DEFAULT (strftime('%s', 'now'))
);
```

**Columns:**
- `id` - Unique board identifier (auto-increment)
- `name` - Board name (unique, required)
- `description` - Board description (optional)
- `created_at` - Unix timestamp of creation

**Indexes:**
- Primary key on `id`
- Unique index on `name`

**Example Data:**
```sql
INSERT INTO boards (name, description) VALUES 
    ('General', 'General discussion'),
    ('Technology', 'Tech news and discussions'),
    ('Random', 'Random topics');
```

#### 2. threads

Stores thread/topic headers.

```sql
CREATE TABLE threads (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    board_id INTEGER NOT NULL,
    subject TEXT NOT NULL,
    created_at INTEGER DEFAULT (strftime('%s', 'now')),
    FOREIGN KEY (board_id) REFERENCES boards(id) ON DELETE CASCADE
);
```

**Columns:**
- `id` - Unique thread identifier (auto-increment)
- `board_id` - Foreign key to boards table
- `subject` - Thread title/subject (required)
- `created_at` - Unix timestamp of creation

**Important Notes:**
- Thread content is stored in the `posts` table, not here
- The first post (OP) in a thread contains the thread content
- `author` is NOT stored in threads table

**Indexes:**
- Primary key on `id`
- Foreign key index on `board_id`

**Example Data:**
```sql
INSERT INTO threads (board_id, subject) VALUES 
    (1, 'Welcome to the forum!'),
    (2, 'New programming language released');
```

#### 3. posts

Stores all posts, including the original post (OP) of each thread.

```sql
CREATE TABLE posts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    thread_id INTEGER NOT NULL,
    author TEXT NOT NULL,
    content TEXT NOT NULL,
    reply_to INTEGER,
    created_at INTEGER DEFAULT (strftime('%s', 'now')),
    FOREIGN KEY (thread_id) REFERENCES threads(id) ON DELETE CASCADE,
    FOREIGN KEY (reply_to) REFERENCES posts(id) ON DELETE SET NULL
);
```

**Columns:**
- `id` - Unique post identifier (auto-increment)
- `thread_id` - Foreign key to threads table
- `author` - Post author name (required)
- `content` - Post content/body (required)
- `reply_to` - Optional foreign key to another post (for replies)
- `created_at` - Unix timestamp of creation

**Important Notes:**
- First post in a thread (`MIN(id)` for that `thread_id`) is the OP
- `reply_to` can be NULL for OP or top-level replies
- `reply_to` creates a tree structure for threaded conversations

**Indexes:**
- Primary key on `id`
- Foreign key index on `thread_id`
- Foreign key index on `reply_to`

**Example Data:**
```sql
-- Original post (OP)
INSERT INTO posts (thread_id, author, content) VALUES 
    (1, 'Admin', 'This is the first post of the thread');

-- Reply to OP
INSERT INTO posts (thread_id, author, content, reply_to) VALUES 
    (1, 'User123', 'Thanks for the welcome!', 1);
```

#### 4. sessions

Stores user session tokens for authentication.

```sql
CREATE TABLE sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    token TEXT NOT NULL UNIQUE,
    created_at INTEGER DEFAULT (strftime('%s', 'now')),
    expires_at INTEGER NOT NULL
);
```

**Columns:**
- `id` - Unique session identifier (auto-increment)
- `user_id` - User identifier (admin = 1)
- `token` - Unique session token (random string)
- `created_at` - Unix timestamp of session creation
- `expires_at` - Unix timestamp when session expires

**Indexes:**
- Primary key on `id`
- Unique index on `token`

**Example Data:**
```sql
INSERT INTO sessions (user_id, token, expires_at) VALUES 
    (1, 'abc123...', strftime('%s', 'now') + 86400);  -- 24 hour expiry
```

## Relationships

### Entity Relationship Diagram

```
┌─────────┐
│ boards  │
│─────────│
│ id (PK) │
│ name    │
│ desc    │
└────┬────┘
     │
     │ 1:N
     │
┌────▼──────┐
│ threads   │
│───────────│
│ id (PK)   │
│ board_id  │◄────────┐
│ subject   │         │
└────┬──────┘         │
     │                │
     │ 1:N            │
     │                │
┌────▼──────┐         │
│ posts     │         │
│───────────│         │
│ id (PK)   │         │
│ thread_id │─────────┘
│ author    │
│ content   │
│ reply_to  │──────┐
└───────────┘      │
     ▲             │
     └─────────────┘
        (self-reference)

┌───────────┐
│ sessions  │
│───────────│
│ id (PK)   │
│ user_id   │
│ token     │
│ expires   │
└───────────┘
```

## Data Access Patterns

### Common Queries

#### List boards with thread count
```sql
SELECT b.id, b.name, b.description, COUNT(t.id) as thread_count
FROM boards b
LEFT JOIN threads t ON b.id = t.board_id
GROUP BY b.id
ORDER BY b.name;
```

#### Get thread with first post (OP)
```sql
SELECT 
    t.id, t.subject, t.created_at,
    p.author, p.content
FROM threads t
JOIN posts p ON t.id = p.thread_id
WHERE t.id = ? 
    AND p.id = (SELECT MIN(id) FROM posts WHERE thread_id = t.id);
```

#### Get all posts in a thread
```sql
SELECT id, author, content, reply_to, created_at
FROM posts
WHERE thread_id = ?
ORDER BY created_at ASC;
```

#### Check valid session
```sql
SELECT user_id
FROM sessions
WHERE token = ? 
    AND expires_at > strftime('%s', 'now');
```

## Migrations

### Migration System

The application uses a simple migration system in `db_migrate()`:

```c
int db_migrate(void) {
    // Create boards table
    db_exec("CREATE TABLE IF NOT EXISTS boards (...)");
    
    // Create threads table
    db_exec("CREATE TABLE IF NOT EXISTS threads (...)");
    
    // Create posts table
    db_exec("CREATE TABLE IF NOT EXISTS posts (...)");
    
    // Create sessions table
    db_exec("CREATE TABLE IF NOT EXISTS sessions (...)");
    
    return 0;
}
```

### Future Migration Strategy

For schema changes in future versions:

1. Add version tracking:
```sql
CREATE TABLE schema_version (
    version INTEGER PRIMARY KEY
);
```

2. Implement incremental migrations:
```c
int migrate_to_v2(void) {
    // Add new column
    db_exec("ALTER TABLE posts ADD COLUMN edited_at INTEGER");
    return 0;
}
```

3. Track applied migrations:
```c
int current_version = get_schema_version();
if (current_version < 2) {
    migrate_to_v2();
    set_schema_version(2);
}
```

## Data Integrity

### Foreign Key Constraints

**Enabled by default in SQLite 3.46.1**

- `threads.board_id` → `boards.id` (CASCADE on delete)
- `posts.thread_id` → `threads.id` (CASCADE on delete)
- `posts.reply_to` → `posts.id` (SET NULL on delete)

### Cascade Behavior

**When a board is deleted:**
- All threads in that board are deleted
- All posts in those threads are deleted

**When a thread is deleted:**
- All posts in that thread are deleted

**When a post is deleted:**
- Replies to that post have `reply_to` set to NULL

## Performance Considerations

### Indexing Strategy

**Current indexes:**
- Primary keys (automatic)
- Foreign keys (automatic in SQLite)
- Unique constraint on `boards.name`
- Unique constraint on `sessions.token`

**Recommended future indexes:**
```sql
-- For board view (list threads by date)
CREATE INDEX idx_threads_board_created 
ON threads(board_id, created_at DESC);

-- For thread view (list posts by date)
CREATE INDEX idx_posts_thread_created 
ON posts(thread_id, created_at ASC);

-- For session cleanup
CREATE INDEX idx_sessions_expires 
ON sessions(expires_at);
```

### Query Optimization

**Use prepared statements:**
```c
sqlite3_stmt *stmt;
db_prepare("SELECT * FROM posts WHERE thread_id = ?", &stmt);
sqlite3_bind_int64(stmt, 1, thread_id);
while (db_step(stmt) == SQLITE_ROW) {
    // Process row
}
db_finalize(stmt);
```

**Batch inserts:**
```c
db_exec("BEGIN TRANSACTION");
for (int i = 0; i < count; i++) {
    insert_post(...);
}
db_exec("COMMIT");
```

## Backup and Maintenance

### Backup Strategy

**Online backup (while running):**
```bash
sqlite3 app.db ".backup app.db.backup"
```

**WAL checkpoint:**
```sql
PRAGMA wal_checkpoint(FULL);
```

### Database Cleanup

**Remove expired sessions:**
```sql
DELETE FROM sessions WHERE expires_at < strftime('%s', 'now');
```

**Vacuum (reclaim space):**
```sql
VACUUM;
```

**Analyze (update statistics):**
```sql
ANALYZE;
```

## Security

### SQL Injection Prevention

**Always use prepared statements:**
```c
// GOOD
db_prepare("SELECT * FROM boards WHERE id = ?", &stmt);
sqlite3_bind_int64(stmt, 1, board_id);

// BAD - Never do this!
char query[256];
snprintf(query, sizeof(query), "SELECT * FROM boards WHERE id = %lld", board_id);
db_exec(query);
```

### Session Security

- Tokens are randomly generated (128 characters)
- Tokens stored as plain text (consider hashing for production)
- Expired sessions automatically rejected
- Session expiry: 24 hours default

## Troubleshooting

### Common Issues

**Database locked:**
- Increase busy timeout: `sqlite3_busy_timeout(db, 10000)`
- Ensure WAL mode is enabled
- Check for long-running transactions

**ENOENT errors:**
- Database directory doesn't exist
- Solution: `ensure_directory_exists()` in `db_init()`

**Segmentation fault:**
- Memory-mapped I/O issue
- Solution: `PRAGMA mmap_size=0`

**Slow queries:**
- Missing indexes
- Solution: Add indexes on frequently queried columns

## Related Documentation

- [BUGFIX_SUMMARY.md](BUGFIX_SUMMARY.md) - Database stability fixes
- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture
- [README.md](../README.md) - General project information
