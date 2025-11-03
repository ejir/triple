# Bug Fix Summary

## Issues Fixed

### 1. Create New Board Returns 404 Error
**Problem:** The "Create New Board" form on the homepage was submitting to `/board/create` endpoint, but this route was not registered in the router, causing a 404 Not Found error.

**Solution:**
- Added `board_create_handler()` function to handle POST requests to `/board/create`
- Registered the route in `board_register_routes()` 
- The handler parses form data, validates input, and creates a new board in the database
- Returns a success page with a link to view the newly created board

### 2. Chinese/UTF-8 Character Encoding Issue
**Problem:** Chinese characters and other UTF-8 encoded text were being double-encoded and displayed incorrectly (e.g., showing as `%26%2326631%3B%26%2335760%3B` instead of the actual characters).

**Solution:**
- **Added URL decoding:** Created `url_decode()` and `hex_to_int()` helper functions to properly decode URL-encoded form data
- **Updated all form handlers:** Modified `board_create_handler()`, `thread_create_handler()`, and `post_create_handler()` to use `url_decode()` when parsing form field values
- **Added UTF-8 meta tags:** Added `<meta charset="UTF-8">` to all HTML page headers
- **Added charset to HTTP headers:** Modified `send_response()` in `http.c` to automatically append `; charset=utf-8` to `text/html` Content-Type headers

## Files Modified

### src/board.c
- Added `hex_to_int()` helper function for URL decoding
- Added `url_decode()` function to decode URL-encoded strings
- Added `board_create_handler()` to handle board creation
- Updated `board_register_routes()` to register the `/board/create` route
- Modified `thread_create_handler()` to use `url_decode()` for form fields
- Modified `post_create_handler()` to use `url_decode()` for form fields
- Added UTF-8 meta charset tags to HTML output in `board_list_handler()`, `board_view_handler()`, and `thread_view_handler()`

### src/board.h
- Added declaration for `board_create_handler()`

### src/http.c
- Modified `send_response()` to automatically add `charset=utf-8` to text/html Content-Type headers

## Technical Details

### URL Decoding Algorithm
The `url_decode()` function handles:
- Percent-encoded characters (e.g., `%E4%B8%AD` → 中)
- Plus signs as spaces (`+` → ` `)
- Normal characters pass through unchanged

This ensures that UTF-8 multi-byte characters submitted via HTML forms are correctly decoded and stored in the database.

### Character Set Support
- All HTML pages now include `<meta charset="UTF-8">` in the head section
- HTTP responses include `Content-Type: text/html; charset=utf-8` header
- SQLite3 stores text as UTF-8 by default (no changes needed)

## Testing

Both issues have been verified as fixed:

1. **Board Creation:** 
   - Endpoint `/board/create` returns 200 OK instead of 404
   - Boards can be created successfully via the web form

2. **UTF-8 Support:**
   - Chinese characters display correctly in all fields (board name, title, description, thread subject, author, post content)
   - Characters are properly encoded in database and rendered in HTML
   - URL-encoded form data is correctly decoded

### Example Test Commands
```bash
# Test board creation with Chinese characters
curl -X POST http://localhost:8080/board/create \
  -d "name=测试板&title=测试一下&description=这是中文描述"

# Test thread creation with Chinese characters
curl -X POST http://localhost:8080/thread \
  -d "board_id=1&subject=测试主题&author=张三&content=这是测试内容"

# Test post creation with Chinese characters
curl -X POST http://localhost:8080/post \
  -d "thread_id=1&author=李四&content=我也来回复一下"
```

## Compatibility

- Changes are backward compatible
- No database schema changes required
- No breaking changes to existing functionality
- Works with both Cosmopolitan and GCC builds
