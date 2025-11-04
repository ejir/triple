# API Documentation

## Overview

This document describes the HTTP endpoints and request/response formats for the Cosmopolitan web application.

## Base URL

```
http://localhost:8080
```

## Content Types

- **HTML Responses**: `text/html; charset=utf-8`
- **Form Submissions**: `application/x-www-form-urlencoded`
- **File Uploads**: `multipart/form-data`

## Common Parameters

### Language Parameter

All GET requests support the `lang` parameter for internationalization:

```
?lang=en        # English
?lang=zh-cn     # Simplified Chinese
```

The language preference is also stored in a cookie (`lang`) with 1-year expiry.

## Public Endpoints

### Board Listing

**GET /**

List all message boards.

**Parameters:**
- `lang` (optional) - Language code (en, zh-cn)

**Response:**
```html
<!DOCTYPE html>
<html>
  <head>
    <title>Message Boards</title>
    ...
  </head>
  <body>
    <div class="board-list">
      <div class="board-item">
        <h2>General</h2>
        <p>General discussion</p>
        <a href="/board?id=1">View Board</a>
      </div>
      ...
    </div>
  </body>
</html>
```

**Status Codes:**
- `200 OK` - Success

---

### Board View

**GET /board**

View a specific board with its threads.

**Parameters:**
- `id` (required) - Board ID (integer)
- `lang` (optional) - Language code

**Example:**
```
GET /board?id=1&lang=en
```

**Response:**
```html
<!DOCTYPE html>
<html>
  <head>
    <title>General - Board</title>
    ...
  </head>
  <body>
    <h1>General</h1>
    <p>General discussion</p>
    
    <!-- Thread creation form -->
    <form method="POST" action="/thread">
      <input type="hidden" name="board_id" value="1">
      <input type="text" name="subject" required>
      <textarea name="content" required></textarea>
      <button type="submit">Create Thread</button>
    </form>
    
    <!-- Thread list -->
    <div class="thread-list">
      <div class="thread-item">
        <h3><a href="/thread?id=1">Welcome!</a></h3>
        <p>By Admin - 2 hours ago</p>
      </div>
      ...
    </div>
  </body>
</html>
```

**Status Codes:**
- `200 OK` - Success
- `404 Not Found` - Board not found

---

### Thread View

**GET /thread**

View a specific thread with all its posts.

**Parameters:**
- `id` (required) - Thread ID (integer)
- `lang` (optional) - Language code

**Example:**
```
GET /thread?id=1&lang=en
```

**Response:**
```html
<!DOCTYPE html>
<html>
  <head>
    <title>Welcome! - Thread</title>
    ...
  </head>
  <body>
    <h1>Welcome!</h1>
    
    <!-- Original post -->
    <div class="post original-post">
      <div class="post-author">Admin</div>
      <div class="post-content">Welcome to the forum!</div>
      <div class="post-time">2 hours ago</div>
    </div>
    
    <!-- Replies -->
    <div class="post-list">
      <div class="post">
        <div class="post-author">User123</div>
        <div class="post-content">Thanks! (^_^)</div>
        <div class="post-time">1 hour ago</div>
      </div>
      ...
    </div>
    
    <!-- Reply form -->
    <form method="POST" action="/post">
      <input type="hidden" name="thread_id" value="1">
      <input type="text" name="author" required>
      <textarea name="content" required></textarea>
      <button type="submit">Reply</button>
    </form>
  </body>
</html>
```

**Status Codes:**
- `200 OK` - Success
- `404 Not Found` - Thread not found

---

### Create Thread

**POST /thread**

Create a new thread in a board.

**Content-Type:** `application/x-www-form-urlencoded`

**Parameters:**
- `board_id` (required) - Board ID (integer)
- `subject` (required) - Thread subject/title
- `author` (required) - Author name
- `content` (required) - Thread content (first post)

**Example Request:**
```http
POST /thread HTTP/1.1
Content-Type: application/x-www-form-urlencoded

board_id=1&subject=New+Thread&author=User123&content=This+is+my+thread
```

**Response:**
- `302 Found` - Redirect to new thread
  - Location: `/thread?id={thread_id}`

**Status Codes:**
- `302 Found` - Success (redirect)
- `400 Bad Request` - Missing required parameters
- `404 Not Found` - Board not found

---

### Create Post

**POST /post**

Create a new post (reply) in a thread.

**Content-Type:** `application/x-www-form-urlencoded`

**Parameters:**
- `thread_id` (required) - Thread ID (integer)
- `author` (required) - Author name
- `content` (required) - Post content
- `reply_to` (optional) - Post ID being replied to

**Example Request:**
```http
POST /post HTTP/1.1
Content-Type: application/x-www-form-urlencoded

thread_id=1&author=User456&content=Great+post!&reply_to=5
```

**Response:**
- `302 Found` - Redirect to thread
  - Location: `/thread?id={thread_id}`

**Status Codes:**
- `302 Found` - Success (redirect)
- `400 Bad Request` - Missing required parameters
- `404 Not Found` - Thread not found

---

## Admin Endpoints

All admin endpoints require authentication via session cookie.

### Admin Login Page

**GET /admin/login**

Display the admin login form.

**Parameters:**
- `lang` (optional) - Language code

**Response:**
```html
<!DOCTYPE html>
<html>
  <head>
    <title>Admin Login</title>
    ...
  </head>
  <body>
    <form method="POST" action="/admin/login">
      <input type="text" name="username" required>
      <input type="password" name="password" required>
      <button type="submit">Login</button>
    </form>
  </body>
</html>
```

**Status Codes:**
- `200 OK` - Success

---

### Admin Login Submit

**POST /admin/login**

Authenticate admin user.

**Content-Type:** `application/x-www-form-urlencoded`

**Parameters:**
- `username` (required) - Admin username
- `password` (required) - Admin password

**Example Request:**
```http
POST /admin/login HTTP/1.1
Content-Type: application/x-www-form-urlencoded

username=admin&password=secret123
```

**Response (Success):**
- `302 Found` - Redirect to dashboard
  - Location: `/admin`
  - Set-Cookie: `session={token}; Path=/; Max-Age=86400`

**Response (Failure):**
- `401 Unauthorized` - Invalid credentials
  - HTML page with error message

**Status Codes:**
- `302 Found` - Success (redirect)
- `401 Unauthorized` - Invalid credentials

---

### Admin Dashboard

**GET /admin**

Display the admin control panel.

**Authentication:** Required (session cookie)

**Parameters:**
- `lang` (optional) - Language code

**Response:**
```html
<!DOCTYPE html>
<html>
  <head>
    <title>Admin Dashboard</title>
    ...
  </head>
  <body>
    <h1>Admin Dashboard</h1>
    
    <!-- Statistics -->
    <div class="stats">
      <div class="stat-item">
        <h3>5</h3>
        <p>Boards</p>
      </div>
      <div class="stat-item">
        <h3>42</h3>
        <p>Threads</p>
      </div>
      <div class="stat-item">
        <h3>157</h3>
        <p>Posts</p>
      </div>
    </div>
    
    <!-- Board management -->
    <div class="board-management">
      <h2>Boards</h2>
      <form method="POST" action="/admin/board">
        <input type="text" name="name" required>
        <textarea name="description"></textarea>
        <button type="submit">Create Board</button>
      </form>
      
      <div class="board-list">
        <div class="board-item">
          <h3>General</h3>
          <form method="POST" action="/admin/board/delete">
            <input type="hidden" name="board_id" value="1">
            <button type="submit">Delete</button>
          </form>
        </div>
        ...
      </div>
    </div>
  </body>
</html>
```

**Status Codes:**
- `200 OK` - Success
- `401 Unauthorized` - Not authenticated
- `302 Found` - Redirect to login if not authenticated

---

### Admin Logout

**GET /admin/logout**

Log out admin user.

**Authentication:** Required (session cookie)

**Response:**
- `302 Found` - Redirect to home
  - Location: `/`
  - Set-Cookie: `session=; Path=/; Max-Age=0` (clear cookie)

**Status Codes:**
- `302 Found` - Success (redirect)

---

### Create Board (Admin)

**POST /admin/board**

Create a new board.

**Authentication:** Required (session cookie)

**Content-Type:** `application/x-www-form-urlencoded`

**Parameters:**
- `name` (required) - Board name (unique)
- `description` (optional) - Board description

**Example Request:**
```http
POST /admin/board HTTP/1.1
Cookie: session=abc123...
Content-Type: application/x-www-form-urlencoded

name=Technology&description=Tech+discussions
```

**Response:**
- `302 Found` - Redirect to admin dashboard
  - Location: `/admin`

**Status Codes:**
- `302 Found` - Success (redirect)
- `400 Bad Request` - Missing name or duplicate name
- `401 Unauthorized` - Not authenticated

---

### Delete Board (Admin)

**POST /admin/board/delete**

Delete a board and all its threads/posts.

**Authentication:** Required (session cookie)

**Content-Type:** `application/x-www-form-urlencoded`

**Parameters:**
- `board_id` (required) - Board ID (integer)

**Example Request:**
```http
POST /admin/board/delete HTTP/1.1
Cookie: session=abc123...
Content-Type: application/x-www-form-urlencoded

board_id=3
```

**Response:**
- `302 Found` - Redirect to admin dashboard
  - Location: `/admin`

**Status Codes:**
- `302 Found` - Success (redirect)
- `400 Bad Request` - Missing board_id
- `401 Unauthorized` - Not authenticated
- `404 Not Found` - Board not found

---

## Upload Endpoints

### Upload File

**POST /upload**

Upload a file to the server.

**Content-Type:** `multipart/form-data`

**Parameters:**
- `file` (required) - File upload field

**Example Request:**
```http
POST /upload HTTP/1.1
Content-Type: multipart/form-data; boundary=----WebKitFormBoundary

------WebKitFormBoundary
Content-Disposition: form-data; name="file"; filename="image.jpg"
Content-Type: image/jpeg

[binary file data]
------WebKitFormBoundary--
```

**Response:**
```html
<!DOCTYPE html>
<html>
  <body>
    <h1>Upload Successful</h1>
    <p>File saved: image.jpg</p>
  </body>
</html>
```

**Status Codes:**
- `200 OK` - Success
- `400 Bad Request` - No file provided
- `413 Payload Too Large` - File too large (if implemented)

---

## Error Responses

### 404 Not Found

**Response:**
```html
<!DOCTYPE html>
<html>
  <head>
    <title>404 - Not Found</title>
  </head>
  <body>
    <h1>404 - Page Not Found</h1>
    <p>The requested resource was not found.</p>
    <a href="/">Back to Home</a>
  </body>
</html>
```

### 401 Unauthorized

**Response:**
```html
<!DOCTYPE html>
<html>
  <head>
    <title>401 - Unauthorized</title>
  </head>
  <body>
    <h1>401 - Unauthorized</h1>
    <p>You must be logged in to access this page.</p>
    <a href="/admin/login">Login</a>
  </body>
</html>
```

---

## Session Management

### Session Cookie

**Name:** `session`

**Format:** Random 128-character alphanumeric string

**Attributes:**
- `Path=/` - Available to all routes
- `Max-Age=86400` - 24 hour expiry (86400 seconds)
- `HttpOnly` - Not accessible via JavaScript (if implemented)
- `Secure` - Only sent over HTTPS (if HTTPS enabled)

**Example:**
```
Set-Cookie: session=abc123def456...; Path=/; Max-Age=86400
```

### Session Validation

Sessions are validated on each admin request:
1. Extract token from `session` cookie
2. Query database: `SELECT user_id FROM sessions WHERE token = ? AND expires_at > now()`
3. If found, user is authenticated
4. If not found or expired, redirect to login

---

## Rate Limiting

Currently not implemented. Future consideration for:
- Login attempts (prevent brute force)
- Post creation (prevent spam)
- File uploads (prevent abuse)

---

## CORS

Currently not configured. All requests must be same-origin.

---

## Future API Endpoints

### JSON API (Planned)

**GET /api/boards**
```json
{
  "boards": [
    {
      "id": 1,
      "name": "General",
      "description": "General discussion",
      "thread_count": 42
    }
  ]
}
```

**GET /api/threads?board_id=1**
```json
{
  "threads": [
    {
      "id": 1,
      "subject": "Welcome!",
      "author": "Admin",
      "post_count": 5,
      "created_at": 1699000000
    }
  ]
}
```

**GET /api/posts?thread_id=1**
```json
{
  "posts": [
    {
      "id": 1,
      "author": "Admin",
      "content": "Welcome to the forum!",
      "reply_to": null,
      "created_at": 1699000000
    }
  ]
}
```

---

## Related Documentation

- [README.md](README.md) - Project overview
- [DATABASE_SCHEMA.md](DATABASE_SCHEMA.md) - Database structure
- [LOCALIZATION.md](LOCALIZATION.md) - Language support
- [ARCHITECTURE.md](ARCHITECTURE.md) - System design
