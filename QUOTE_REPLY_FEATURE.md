# Quote Reply Feature Implementation

## Overview
Implemented a quote reply feature for the message board that allows users to reply to specific posts, with visual indicators and the ability to expand/collapse quoted post content.

## Changes Made

### 1. Database Schema (src/db.c)
- Added `reply_to` column to the `posts` table
- Type: INTEGER (nullable)
- Foreign key reference to `posts(id)`
- Allows tracking which post is being replied to

### 2. Thread View HTML/CSS/JavaScript (src/board.c)

#### CSS Styling
- `.post`: Post container with border
- `.post-header`: Flexbox layout for header (name/id on left, reply button on right)
- `.reply-btn`: Green button styled for reply action
- `.quote-ref`: Blue clickable reference to quoted post (e.g., ">>123")
- `.quoted-post`: Hidden by default, gray background with blue left border
- `.quoted-post.expanded`: Visible state when toggled

#### JavaScript Functions
- `replyToPost(postId)`: Sets the reply_to hidden field and scrolls to reply form
- `toggleQuote(postId)`: Toggles visibility of quoted post content

#### HTML Structure
Each post now displays:
- Post header with author name and post ID (#123)
- If replying to another post: clickable reference (>>123)
- Reply button on the right side
- Quoted post content (hidden by default, can be toggled)
- Post content

### 3. Post Display Logic (src/board.c - thread_view_handler)
- Updated SQL query to LEFT JOIN with posts table to fetch reply_to data
- Query now fetches: post data + referenced post data (id, author, content)
- Displays quote reference link when post has reply_to
- Shows collapsible quoted post content

### 4. Reply Form (src/board.c - thread_view_handler)
- Added hidden `reply_to` field with id="reply_to"
- Added id="content" to textarea for JavaScript focus
- Added id="reply-form" to form for scrolling

### 5. Post Creation Handler (src/board.c - post_create_handler)
- Parse `reply_to` parameter from POST body
- Store reply_to value in database (or NULL if not replying)
- Updated INSERT statement to include reply_to column

### 6. Thread Creation Handler (src/board.c - thread_create_handler)
- Updated initial post creation to include reply_to field (always NULL for OP)

## User Interaction Flow

1. User views a thread with multiple posts
2. Each post has a "Reply" button on the right side
3. User clicks "Reply" on a specific post
4. Browser scrolls to reply form
5. Hidden reply_to field is populated with the post ID
6. User writes their reply and submits
7. New post is created with reference to original post
8. Post displays with ">>123" reference link
9. Clicking ">>123" expands/collapses the quoted post content

## Features

✅ Reply buttons on all posts (right-aligned)
✅ Quote references displayed as clickable links (>>PostID)
✅ Expand/collapse functionality for quoted posts
✅ Smooth scrolling to reply form
✅ Auto-focus on content textarea
✅ Visual styling with colors and borders
✅ Database foreign key relationships
✅ NULL handling for non-reply posts

## Testing

Run `./test_reply_feature.sh` to verify:
- Database schema includes reply_to column
- Posts can be created with reply_to references
- HTML includes all necessary elements (buttons, scripts, styles)
- Quote references are displayed correctly

## Browser Compatibility

Uses standard JavaScript and CSS features:
- classList.toggle() for show/hide
- scrollIntoView() with smooth behavior
- Flexbox for layout
- Works in all modern browsers
