# Security Updates

This document describes the security enhancements implemented in the codebase.

## Changes Summary

### 1. Restricted Board Creation to Admin Only

**Files Modified:** `src/board.c`

- Added authentication check to `board_create_handler()` to ensure only authenticated administrators can create new boards
- Modified `board_list_handler()` to only display the "Create New Board" form to authenticated administrators
- Returns HTTP 403 Forbidden when non-admin users attempt to access board creation

**Implementation Details:**
- Uses `admin_is_authenticated(req)` function to verify admin session
- Form is conditionally rendered based on authentication status
- Explicit error message shown when unauthorized users try to create boards

### 2. Admin Password Change Feature

**Files Modified:** `src/admin.c`, `src/admin.h`

Added new functionality allowing administrators to change their password:
- New route: `/admin/change-password` (GET and POST)
- New handler: `admin_change_password_handler()`
- Link added to admin dashboard for easy access

**Features:**
- Requires current password verification
- New password confirmation to prevent typos
- Password validation before update
- Secure session-based authentication required
- Clear success/error messages

**Helper Function Added:**
- `get_current_user_id()` - Retrieves user ID from session token

### 3. XSS Attack Prevention

**Files Modified:** `src/render.c`, `src/render.h`, `src/board.c`

Implemented comprehensive XSS (Cross-Site Scripting) protection:

#### New Escaping Functions:
1. **`render_escape_html()`** - Escapes HTML special characters
   - Converts: `&`, `<`, `>`, `"`, `'` to HTML entities
   - Used for all user-generated content displayed in HTML context

2. **`render_escape_js()`** - Escapes JavaScript special characters
   - Escapes: `\`, `'`, `"`, newlines, carriage returns, tabs
   - Used for content embedded in JavaScript contexts (e.g., onclick attributes)

#### Content Escaping Applied To:
- Board names, titles, and descriptions
- Thread subjects
- Post content
- Author names
- Kaomoji category titles
- Kaomoji items (both in HTML display and JavaScript onclick handlers)
- Quoted post content

**XSS Protection Strategy:**
- All user-generated content is escaped before rendering
- Separate escaping for HTML and JavaScript contexts
- Memory properly managed (allocated escaped strings are freed)
- Fallback to original content if escaping fails

### 4. Authentication Function Made Public

**Files Modified:** `src/admin.c`, `src/admin.h`

- Renamed `is_authenticated()` to `admin_is_authenticated()`
- Made function public (exported in header file)
- Allows other modules (like board.c) to check admin authentication status

## Security Benefits

1. **Authorization Control**: Prevents unauthorized users from creating boards, ensuring only admins can manage board structure

2. **Password Security**: Allows admins to change compromised passwords, requires verification of current password

3. **XSS Prevention**: Protects against malicious JavaScript injection through user input, preventing:
   - Cookie theft
   - Session hijacking
   - Defacement
   - Phishing attacks
   - Malicious redirects

## Testing Recommendations

1. **Board Creation Authorization:**
   - Verify non-logged-in users cannot see or access board creation form
   - Verify logged-in admin can create boards
   - Test direct POST to `/board/create` without auth returns 403

2. **Password Change:**
   - Test with incorrect current password
   - Test with mismatched new passwords
   - Test successful password change
   - Verify new password works for login

3. **XSS Prevention:**
   - Try injecting `<script>alert('XSS')</script>` in various fields
   - Test special characters in board names, titles, descriptions
   - Test HTML tags in post content and author names
   - Verify kaomoji with special characters display correctly

## Migration Notes

- No database schema changes required
- Existing data remains compatible
- No breaking changes to existing functionality
- All changes are additive security enhancements
