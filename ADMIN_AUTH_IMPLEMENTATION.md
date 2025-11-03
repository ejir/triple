# Admin Authentication Implementation

## Overview
This document describes the implementation of cookie-based authentication for the admin panel.

## Features Implemented

### 1. Cookie Support in HTTP Layer
- Added `cookies` field to `http_request_t` structure
- Added `set_cookie` field to `http_response_t` structure
- Cookie parsing in HTTP request handler
- Set-Cookie header support in HTTP response

### 2. Database Schema
Added two new tables:
- `admin_users`: Stores admin user credentials (username, password)
- `admin_sessions`: Stores active sessions with expiration

Default admin user created on first run:
- Username: `admin`
- Password: `admin`

### 3. Authentication Functions
- `is_authenticated()`: Checks if request has valid session cookie
- `get_cookie_value()`: Extracts cookie value by name
- `generate_session_token()`: Creates random 64-character token
- `create_session()`: Creates new session in database
- `destroy_session()`: Removes session from database

### 4. Login Handler (`/admin/login`)
**GET**: Displays login form with styled HTML
- Redirects to dashboard if already authenticated
- Shows username and password input fields
- Includes CSS styling for better UX

**POST**: Processes login credentials
- Validates username and password against database
- Creates session on successful authentication
- Sets `admin_session` cookie with 7-day expiration
- Redirects to admin dashboard
- Shows error message on invalid credentials

### 5. Logout Handler (`/admin/logout`)
**GET**: Logs out the user
- Destroys session in database
- Clears cookie by setting Max-Age=0
- Shows confirmation page with links

### 6. Dashboard Protection
Admin dashboard now checks authentication:
- Returns 403 Forbidden if not authenticated
- Shows login link for unauthenticated users
- Displays dashboard with logout link for authenticated users

## Security Features
- Session tokens are 64 characters long and randomly generated
- Sessions expire after 7 days
- Cookies are HttpOnly to prevent XSS attacks
- Sessions stored in database for server-side validation

## Cookie Details
- **Name**: `admin_session`
- **Path**: `/`
- **Max-Age**: 604800 seconds (7 days)
- **Flags**: HttpOnly

## Testing
1. Access `/admin` - should show access denied
2. Access `/admin/login` - should show login form
3. Login with `admin` / `admin` - should redirect to dashboard
4. Access `/admin` - should show dashboard with stats
5. Click "Logout" - should clear session and show logout page
6. Try to access `/admin` again - should show access denied

## Code Changes
- `src/http.h`: Added cookies and set_cookie fields
- `src/http.c`: Cookie parsing and Set-Cookie header support
- `src/db.c`: Added admin_users and admin_sessions tables
- `src/admin.c`: Complete authentication implementation

## Future Improvements
- Password hashing (currently plain text)
- CSRF protection
- Session cleanup for expired sessions
- Multiple admin users support
- Password reset functionality
