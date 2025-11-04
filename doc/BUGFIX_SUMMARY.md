# Bug Fix Summary: ENOENT/Segfault and CI Release

## Issue Description

The application was experiencing segmentation faults with ENOENT errors during database operations:

```
lseek(3, 20'480, SEEK_SET) → 20'480 ENOENT
read(3, ..., 4'096) → 4'096 ENOENT
fcntl(3, F_SETLK, {.l_type=F_UNLCK}) → 0 ENOENT
mmap(...) → ENOENT
Terminating on uncaught SIGSEGV
```

## Root Cause

1. **Database file not found**: The database file or its directory didn't exist, causing ENOENT errors
2. **Memory-mapped I/O issues**: SQLite's mmap feature caused segfaults when files were deleted or moved during operation
3. **Missing directory creation**: Upload and database directories were not automatically created

## Solutions Implemented

### 1. Database Module (`src/db.c`)

#### Added Directory Creation
- Implemented `ensure_directory_exists()` function to create database directory if needed
- Validates directory path before database initialization
- Handles `EEXIST` error gracefully

#### Disabled Memory-Mapped I/O
```c
PRAGMA mmap_size=0;
```
- Prevents segfaults caused by accessing deleted/moved files via mmap
- Sacrifices some performance for stability

#### Enabled WAL Mode
```c
PRAGMA journal_mode=WAL;
```
- Write-Ahead Logging for better concurrency
- Reduces database lock contention
- Allows readers and writers to operate concurrently

#### Improved Synchronous Mode
```c
PRAGMA synchronous=NORMAL;
```
- Balances durability and performance
- Provides good crash safety with better performance than FULL mode

#### Added Busy Timeout
```c
sqlite3_busy_timeout(db_conn, 5000);
```
- 5-second timeout for handling database locks
- Prevents immediate failures on busy database

### 2. Upload Module (`src/upload.c`)

#### Auto-create Upload Directory
- Check if upload directory exists on initialization
- Create directory with permissions 0755 if it doesn't exist
- Log warnings if creation fails without breaking the application

### 3. CI/CD Enhancement (`.github/workflows/ci.yml`)

#### Added GitHub Release Creation
- Automatically creates releases when tags are pushed
- Uses `softprops/action-gh-release@v1` action
- Includes:
  - Compiled binary (`app.com`)
  - SHA256 checksums (`SHA256SUMS`)
  - Auto-generated release notes from commits

#### Release Trigger
```yaml
if: startsWith(github.ref, 'refs/tags/')
```
- Only triggers on tag push (e.g., `git tag v1.0.0 && git push origin v1.0.0`)

## Benefits

### Reliability
- ✅ Eliminates ENOENT-related segfaults
- ✅ Graceful handling of missing directories
- ✅ Better error messages for debugging

### Database Performance
- ✅ WAL mode improves concurrency
- ✅ Normal synchronous mode reduces I/O overhead
- ✅ Busy timeout prevents lock conflicts

### CI/CD
- ✅ Automated release process
- ✅ Consistent versioning with tags
- ✅ Easy distribution of binaries

## Testing

### Verify Database Fixes
```bash
# Build the application
make clean && make BUILD_MODE=gcc

# Run without creating directories first
rm -rf app.db uploads
./app.com

# Should automatically create:
# - Database file (app.db)
# - Upload directory (./uploads)
```

### Verify CI Release
```bash
# Create a new tag
git tag v1.0.0

# Push tag to trigger release
git push origin v1.0.0

# Check GitHub Releases page for new release with:
# - app.com binary
# - SHA256SUMS file
# - Auto-generated release notes
```

## Migration Notes

### For Existing Deployments
1. No database migration required - changes are runtime only
2. Existing database files will be upgraded with new PRAGMAs on next startup
3. Upload directory will be created if missing

### For Windows Users
The path in the error trace showed Windows path (`C:/Users/...`). These fixes help with:
- File system differences between Windows and Unix
- Path handling inconsistencies
- Directory creation across platforms

## Future Improvements

### Potential Enhancements
1. Add configurable mmap size for better performance on stable file systems
2. Implement database connection pooling for multi-threaded scenarios
3. Add health check endpoint to monitor database status
4. Implement automatic database backup before migrations

### Performance Tuning
If performance becomes an issue after disabling mmap:
```c
// Instead of disabling entirely, use a smaller size
PRAGMA mmap_size=67108864;  // 64MB
```

## References

- [SQLite Memory-Mapped I/O](https://www.sqlite.org/mmap.html)
- [SQLite WAL Mode](https://www.sqlite.org/wal.html)
- [GitHub Actions Release](https://github.com/softprops/action-gh-release)
