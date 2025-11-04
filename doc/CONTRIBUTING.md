# Contributing Guidelines

Thank you for your interest in contributing to this project! This document provides guidelines and instructions for contributing.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Workflow](#development-workflow)
- [Code Style](#code-style)
- [Testing](#testing)
- [Submitting Changes](#submitting-changes)

## Code of Conduct

- Be respectful and inclusive
- Focus on constructive feedback
- Help others learn and grow
- Keep discussions professional

## Getting Started

### Prerequisites

1. Install the Cosmopolitan toolchain (see README.md)
2. Clone the repository
3. Build the project to ensure everything works:

```bash
make
./app.com
```

### Development Environment

Any text editor or IDE that supports C development will work. Recommended:

- VSCode with C/C++ extension
- Vim/Neovim with appropriate plugins
- Emacs with c-mode
- CLion

## Development Workflow

### 1. Create a Branch

```bash
git checkout -b feature/your-feature-name
# or
git checkout -b fix/your-bug-fix
```

Branch naming conventions:
- `feature/` - New features
- `fix/` - Bug fixes
- `refactor/` - Code refactoring
- `docs/` - Documentation updates
- `test/` - Test additions or modifications

### 2. Make Changes

- Follow the code style guidelines (see below)
- Keep commits focused and atomic
- Write clear commit messages
- Test your changes thoroughly

### 3. Commit Your Changes

Use conventional commit format:

```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types:**
- `feat` - New feature
- `fix` - Bug fix
- `docs` - Documentation changes
- `style` - Code style changes (formatting, etc.)
- `refactor` - Code refactoring
- `test` - Adding or updating tests
- `chore` - Maintenance tasks

**Example:**

```bash
git commit -m "feat(router): add pattern matching support

Implement regex-based pattern matching for routes to allow
more flexible URL handling. This enables dynamic route
parameters and wildcard matching.

Closes #42"
```

### 4. Push and Create Pull Request

```bash
git push origin feature/your-feature-name
```

Then create a pull request on the repository.

## Code Style

### Formatting Rules

**Indentation:**
- Use 4 spaces for indentation
- No tabs
- Continuation lines indent by 4 spaces

**Line Length:**
- Maximum 100 characters per line
- Break long lines at logical points

**Braces:**
- K&R style: opening brace on same line
- Closing brace on its own line
- Always use braces, even for single-statement blocks

```c
// Good
if (condition) {
    do_something();
}

// Bad
if (condition)
    do_something();
```

**Spacing:**
- Space after keywords: `if (`, `for (`, `while (`
- Space around operators: `a + b`, `x = y`
- No space before semicolons: `func();`
- Space after commas: `func(a, b, c)`

**Naming Conventions:**

```c
// Functions: snake_case
int calculate_sum(int a, int b);

// Variables: snake_case
int user_count = 0;
const char *file_name = "data.txt";

// Structs: snake_case with _t suffix
typedef struct {
    int id;
    char *name;
} user_t;

// Enums: snake_case with UPPER_CASE values
typedef enum {
    STATUS_OK,
    STATUS_ERROR,
    STATUS_PENDING
} status_t;

// Constants and Macros: UPPER_SNAKE_CASE
#define MAX_BUFFER_SIZE 1024
#define PI 3.14159

// Preprocessor guards: UPPER_SNAKE_CASE
#ifndef MODULE_H
#define MODULE_H
```

### File Organization

**Header files (.h):**

```c
#ifndef MODULE_H
#define MODULE_H

// System includes
#include <stdio.h>
#include <stdlib.h>

// Project includes
#include "other_module.h"

// Type definitions
typedef struct { ... } type_t;

// Function declarations
int function_name(void);

#endif
```

**Source files (.c):**

```c
// Module header (always first)
#include "module.h"

// System includes
#include <stdio.h>
#include <string.h>

// Project includes
#include "other_module.h"

// Static (private) functions
static int helper_function(void) {
    // Implementation
}

// Public functions
int public_function(void) {
    // Implementation
}
```

### Comments

**When to comment:**
- Complex algorithms
- Non-obvious optimizations
- Workarounds for bugs
- Public API functions
- Important assumptions

**When NOT to comment:**
- Obvious code
- Redundant descriptions

```c
// Good: Explains why
// Use binary search because the array is pre-sorted
int result = binary_search(array, target);

// Bad: States the obvious
// Call binary_search function
int result = binary_search(array, target);
```

**Function documentation:**

```c
/*
 * Calculate the sum of two integers.
 *
 * @param a: First integer
 * @param b: Second integer
 * @return: Sum of a and b
 */
int calculate_sum(int a, int b) {
    return a + b;
}
```

### Error Handling

**Always check return values:**

```c
// Good
FILE *fp = fopen("file.txt", "r");
if (!fp) {
    fprintf(stderr, "Failed to open file: %s\n", strerror(errno));
    return -1;
}

// Bad
FILE *fp = fopen("file.txt", "r");
fread(buffer, 1, size, fp);  // No error check!
```

**Use consistent error codes:**
- `0` for success
- `-1` or specific error codes for failures
- `NULL` for pointer failures

**Clean up resources:**

```c
int process_file(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        return -1;
    }
    
    char *buffer = malloc(1024);
    if (!buffer) {
        fclose(fp);
        return -1;
    }
    
    // Process file...
    
    free(buffer);
    fclose(fp);
    return 0;
}
```

### Memory Management

**Rules:**
1. Every `malloc` must have a corresponding `free`
2. Check all allocations for `NULL`
3. Set pointers to `NULL` after freeing
4. Use `strdup` carefully (it allocates memory)
5. Document ownership of allocated memory

```c
// Good
char *string = malloc(100);
if (!string) {
    return NULL;
}
// Use string...
free(string);
string = NULL;

// Bad
char *string = malloc(100);
// Use string... (no NULL check!)
free(string);
// string still points to freed memory
```

## Testing

### Building and Testing

```bash
# Build the project
make clean
make

# Run the application
./app.com

# Test different scenarios
# - Start and stop the server
# - Access different routes
# - Check error handling
```

### Manual Testing Checklist

- [ ] Application compiles without warnings
- [ ] Application starts successfully
- [ ] All modules initialize correctly
- [ ] HTTP server responds to requests
- [ ] Routes are dispatched correctly
- [ ] Database operations work
- [ ] No memory leaks (use valgrind if available)
- [ ] Application shuts down cleanly

### Static Analysis

Run any available static analysis tools:

```bash
# If available on your system
cppcheck src/
scan-build make
```

## Submitting Changes

### Pull Request Process

1. **Update Documentation**: Ensure README.md and other docs are updated
2. **Test Thoroughly**: Verify all functionality works
3. **Clean Commit History**: Rebase if needed to keep history clean
4. **Write Clear PR Description**:
   - What changes were made
   - Why the changes were necessary
   - How to test the changes
   - Any breaking changes

### PR Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
Describe how you tested your changes

## Checklist
- [ ] Code follows project style guidelines
- [ ] Comments added for complex code
- [ ] Documentation updated
- [ ] No new warnings introduced
- [ ] All tests pass
```

### Review Process

- Maintainers will review your PR
- Address any feedback or requested changes
- Once approved, your PR will be merged

## Questions?

If you have questions or need help:

1. Check existing issues
2. Review documentation
3. Create a new issue with the `question` label

Thank you for contributing!
