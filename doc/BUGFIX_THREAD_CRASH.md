# Bug Fix: Thread View SIGSEGV Crash

## Problem Description

The application was crashing with SIGSEGV (Segmentation Fault) when accessing a thread page after creation. The error manifested as:

```
win32 vectored exception 0xC0000005u raising SIGSEGV
Terminating on uncaught SIGSEGV
```

This crash occurred specifically when:
1. A new thread was created successfully
2. The user navigated to view the thread (`/thread?id=1`)
3. The application would crash during database operations

## Root Cause

The issue was caused by **incorrect parameter ordering** in the `snprintf()` call within the `thread_view_handler()` function in `src/board.c`.

### The Problematic Code

The format string expected parameters in this order:
1. `%s` - Thread subject (for `<title>`)
2. `%lld` - Thread ID (for JavaScript `setLanguage()` function)
3. `%s` - Thread subject (for `<h1>`)
4. `%s` - Language style (English button)
5. `%s` - Language style (Chinese button)
6. `%lld` - Board ID
7. `%s` - Back to board text
8. `%s` - All boards text
9. `%s` - Author name
10. `%s` - Content
11. `%s` - Posts text

However, the actual parameters were passed in the wrong order:
```c
(long long)thread_id,              // Expected %s, got %lld
escaped_subject_title,             // Expected %lld, got %s
escaped_subject_h1,
...
```

This caused:
- **Type mismatch**: A `long long int` was interpreted as a `char *` pointer
- **Invalid memory access**: The integer value was used as a memory address
- **SIGSEGV**: Accessing invalid memory caused a segmentation fault

## The Fix

Corrected the parameter order to match the format string:

```c
escaped_subject_title ? escaped_subject_title : "Thread",  // For <title>
(long long)thread_id,                                       // For JavaScript
escaped_subject_h1 ? escaped_subject_h1 : "Thread",        // For <h1>
(lang == LANG_EN ? "background:rgba(255,255,255,0.2);" : ""),
(lang == LANG_ZH_CN ? "background:rgba(255,255,255,0.2);" : ""),
(long long)thread->board_id,
i18n_get(lang, "back_to_board"),
i18n_get(lang, "all_boards"),
escaped_author ? escaped_author : i18n_get(lang, "anonymous"),
escaped_content ? escaped_content : "No content",
i18n_get(lang, "posts")
```

## File Changed

- `src/board.c` (lines 836-870): Fixed parameter order in `thread_view_handler()`

## Testing

Created comprehensive test suite (`test_thread_crash.sh`) that verifies:

1. ✅ Thread creation succeeds
2. ✅ Thread view loads without crash (critical test)
3. ✅ Application remains stable after accessing thread
4. ✅ Multiple threads can be created and accessed without issues

All tests pass successfully.

## Compilation

The fix also resolved compiler warnings:
- Before: `warning: format '%s' expects argument of type 'char *', but argument 4 has type 'long long int'`
- After: No format warnings

## Impact

- **Severity**: Critical (application crash)
- **Scope**: All thread view operations
- **User Impact**: Application was completely unusable for viewing threads
- **Status**: ✅ FIXED

## Prevention

To prevent similar issues in the future:
1. Always compile with `-Wall -Wextra` flags
2. Pay careful attention to format string warnings
3. Use IDE/editor with format string validation
4. Add automated tests for critical user paths
5. Consider using safer string formatting alternatives (e.g., C++ iostreams, Rust formatting)

## Related Issues

This bug was likely introduced during the internationalization (i18n) feature implementation when the language switcher was added to the thread view page, increasing the complexity of the format string.
