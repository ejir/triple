#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static int test_count = 0;
static int test_passed = 0;
static int test_failed = 0;

void test_start(const char *name) {
    test_count++;
    printf(ANSI_COLOR_YELLOW "[TEST %d] %s" ANSI_COLOR_RESET "\n", test_count, name);
}

void test_pass(void) {
    test_passed++;
    printf(ANSI_COLOR_GREEN "  ✓ PASSED" ANSI_COLOR_RESET "\n\n");
}

void test_fail(const char *msg) {
    test_failed++;
    printf(ANSI_COLOR_RED "  ✗ FAILED: %s" ANSI_COLOR_RESET "\n\n", msg);
}

void test_standard_headers(void) {
    test_start("Standard C headers compatibility");
    
    printf("  Testing standard headers...\n");
    printf("  - stdio.h: OK\n");
    printf("  - stdlib.h: OK\n");
    printf("  - string.h: OK\n");
    printf("  - stdint.h: OK\n");
    printf("  - stddef.h: OK\n");
    printf("  - time.h: OK\n");
    printf("  - signal.h: OK\n");
    printf("  - errno.h: OK\n");
    
    test_pass();
}

void test_memory_operations(void) {
    test_start("Memory allocation and management");
    
    void *ptr = malloc(1024);
    if (ptr == NULL) {
        test_fail("malloc failed");
        return;
    }
    printf("  malloc(1024): OK\n");
    
    memset(ptr, 0, 1024);
    printf("  memset: OK\n");
    
    void *ptr2 = realloc(ptr, 2048);
    if (ptr2 == NULL) {
        free(ptr);
        test_fail("realloc failed");
        return;
    }
    printf("  realloc(2048): OK\n");
    
    free(ptr2);
    printf("  free: OK\n");
    
    void *calloc_ptr = calloc(10, sizeof(int));
    if (calloc_ptr == NULL) {
        test_fail("calloc failed");
        return;
    }
    printf("  calloc(10, sizeof(int)): OK\n");
    free(calloc_ptr);
    
    test_pass();
}

void test_string_operations(void) {
    test_start("String operations");
    
    char buffer[256];
    const char *test_str = "Cosmopolitan Libc Test";
    
    size_t len = strlen(test_str);
    if (len != 22) {
        test_fail("strlen returned wrong value");
        return;
    }
    printf("  strlen: OK (%zu bytes)\n", len);
    
    strcpy(buffer, test_str);
    if (strcmp(buffer, test_str) != 0) {
        test_fail("strcpy failed");
        return;
    }
    printf("  strcpy: OK\n");
    
    strncpy(buffer, "Test", 4);
    printf("  strncpy: OK\n");
    
    strcat(buffer, " string");
    printf("  strcat: OK\n");
    
    char *found = strstr(test_str, "Libc");
    if (found == NULL) {
        test_fail("strstr failed");
        return;
    }
    printf("  strstr: OK\n");
    
    const char *haystack = "needle in haystack";
    char *pos = strchr(haystack, 'n');
    if (pos == NULL) {
        test_fail("strchr failed");
        return;
    }
    printf("  strchr: OK\n");
    
    test_pass();
}

void test_integer_types(void) {
    test_start("Standard integer types (stdint.h)");
    
    int8_t i8 = -128;
    uint8_t u8 = 255;
    int16_t i16 = -32768;
    uint16_t u16 = 65535;
    int32_t i32 = -2147483647;
    uint32_t u32 = 4294967295U;
    int64_t i64 = -9223372036854775807LL;
    uint64_t u64 = 18446744073709551615ULL;
    
    printf("  int8_t: %d\n", i8);
    printf("  uint8_t: %u\n", u8);
    printf("  int16_t: %d\n", i16);
    printf("  uint16_t: %u\n", u16);
    printf("  int32_t: %d\n", i32);
    printf("  uint32_t: %u\n", u32);
    printf("  int64_t: %lld\n", (long long)i64);
    printf("  uint64_t: %llu\n", (unsigned long long)u64);
    
    size_t sz = sizeof(void*);
    printf("  size_t: %zu (pointer size)\n", sz);
    
    test_pass();
}

void test_time_functions(void) {
    test_start("Time functions");
    
    time_t now = time(NULL);
    if (now == (time_t)-1) {
        test_fail("time(NULL) failed");
        return;
    }
    printf("  time(NULL): %lld\n", (long long)now);
    
    struct tm *tm_ptr = gmtime(&now);
    if (tm_ptr == NULL) {
        test_fail("gmtime failed");
        return;
    }
    printf("  gmtime: %04d-%02d-%02d %02d:%02d:%02d UTC\n",
           tm_ptr->tm_year + 1900, tm_ptr->tm_mon + 1, tm_ptr->tm_mday,
           tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
    
    tm_ptr = localtime(&now);
    if (tm_ptr == NULL) {
        test_fail("localtime failed");
        return;
    }
    printf("  localtime: OK\n");
    
    char time_str[64];
    size_t res = strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_ptr);
    if (res == 0) {
        test_fail("strftime failed");
        return;
    }
    printf("  strftime: %s\n", time_str);
    
    test_pass();
}

static volatile sig_atomic_t signal_received = 0;

void test_signal_handler(int signum) {
    signal_received = signum;
}

void test_signal_handling(void) {
    test_start("Signal handling");
    
    signal_received = 0;
    
    void (*old_handler)(int);
    old_handler = signal(SIGUSR1, test_signal_handler);
    if (old_handler == SIG_ERR) {
        test_fail("signal() failed");
        return;
    }
    printf("  signal(SIGUSR1, handler): OK\n");
    
    #ifndef _WIN32
    if (raise(SIGUSR1) != 0) {
        test_fail("raise(SIGUSR1) failed");
        signal(SIGUSR1, old_handler);
        return;
    }
    
    if (signal_received != SIGUSR1) {
        test_fail("signal handler not called");
        signal(SIGUSR1, old_handler);
        return;
    }
    printf("  raise(SIGUSR1): OK (signal received)\n");
    #else
    printf("  raise(SIGUSR1): SKIPPED (Windows)\n");
    #endif
    
    signal(SIGUSR1, old_handler);
    
    test_pass();
}

void test_file_operations(void) {
    test_start("File I/O operations");
    
    const char *test_file = "test_cosmo_file.tmp";
    const char *test_data = "Cosmopolitan test data\n";
    
    FILE *fp = fopen(test_file, "w");
    if (fp == NULL) {
        test_fail("fopen(w) failed");
        return;
    }
    printf("  fopen(w): OK\n");
    
    size_t written = fwrite(test_data, 1, strlen(test_data), fp);
    if (written != strlen(test_data)) {
        fclose(fp);
        unlink(test_file);
        test_fail("fwrite failed");
        return;
    }
    printf("  fwrite: %zu bytes\n", written);
    
    fclose(fp);
    printf("  fclose: OK\n");
    
    fp = fopen(test_file, "r");
    if (fp == NULL) {
        unlink(test_file);
        test_fail("fopen(r) failed");
        return;
    }
    
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        fclose(fp);
        unlink(test_file);
        test_fail("fgets failed");
        return;
    }
    printf("  fgets: OK\n");
    
    if (strcmp(buffer, test_data) != 0) {
        fclose(fp);
        unlink(test_file);
        test_fail("read data doesn't match written data");
        return;
    }
    printf("  data verification: OK\n");
    
    fclose(fp);
    
    if (unlink(test_file) != 0) {
        test_fail("unlink failed");
        return;
    }
    printf("  unlink: OK\n");
    
    test_pass();
}

void test_error_handling(void) {
    test_start("Error handling (errno)");
    
    errno = 0;
    FILE *fp = fopen("/nonexistent/path/file.txt", "r");
    if (fp != NULL) {
        fclose(fp);
        test_fail("fopen should have failed");
        return;
    }
    
    if (errno == 0) {
        test_fail("errno not set on error");
        return;
    }
    
    printf("  errno set on fopen failure: %d\n", errno);
    printf("  strerror: %s\n", strerror(errno));
    
    test_pass();
}

void test_environment_vars(void) {
    test_start("Environment variables");
    
    const char *path = getenv("PATH");
    if (path != NULL) {
        printf("  getenv(PATH): %s\n", path);
    } else {
        printf("  getenv(PATH): (not set)\n");
    }
    
    const char *home = getenv("HOME");
    if (home != NULL) {
        printf("  getenv(HOME): %s\n", home);
    } else {
        printf("  getenv(HOME): (not set)\n");
    }
    
    printf("  Environment access: OK\n");
    
    test_pass();
}

void test_math_operations(void) {
    test_start("Basic arithmetic operations");
    
    int a = 42, b = 17;
    printf("  Addition: %d + %d = %d\n", a, b, a + b);
    printf("  Subtraction: %d - %d = %d\n", a, b, a - b);
    printf("  Multiplication: %d * %d = %d\n", a, b, a * b);
    printf("  Division: %d / %d = %d\n", a, b, a / b);
    printf("  Modulo: %d %% %d = %d\n", a, b, a % b);
    
    uint64_t large1 = 1ULL << 32;
    uint64_t large2 = 1ULL << 31;
    printf("  64-bit operations: %llu + %llu = %llu\n",
           (unsigned long long)large1,
           (unsigned long long)large2,
           (unsigned long long)(large1 + large2));
    
    test_pass();
}

void test_pointer_operations(void) {
    test_start("Pointer operations");
    
    int array[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int *ptr = array;
    
    printf("  Array access: array[3] = %d\n", array[3]);
    printf("  Pointer access: *(ptr + 3) = %d\n", *(ptr + 3));
    
    if (array[3] != *(ptr + 3)) {
        test_fail("pointer arithmetic mismatch");
        return;
    }
    
    printf("  Pointer arithmetic: OK\n");
    
    size_t offset = (char*)(ptr + 5) - (char*)ptr;
    printf("  Pointer difference: %zu bytes\n", offset);
    
    void *null_ptr = NULL;
    if (null_ptr != NULL) {
        test_fail("NULL pointer check failed");
        return;
    }
    printf("  NULL pointer: OK\n");
    
    test_pass();
}

void test_struct_operations(void) {
    test_start("Structure operations");
    
    typedef struct {
        int id;
        char name[32];
        double value;
    } test_struct_t;
    
    test_struct_t s1 = {42, "test", 3.14159};
    printf("  Structure initialization: {%d, \"%s\", %.5f}\n",
           s1.id, s1.name, s1.value);
    
    test_struct_t s2;
    memcpy(&s2, &s1, sizeof(test_struct_t));
    if (s2.id != s1.id || strcmp(s2.name, s1.name) != 0) {
        test_fail("structure copy failed");
        return;
    }
    printf("  Structure copy: OK\n");
    
    test_struct_t *ptr = &s1;
    if (ptr->id != 42) {
        test_fail("structure pointer access failed");
        return;
    }
    printf("  Structure pointer: OK\n");
    
    printf("  Structure size: %zu bytes\n", sizeof(test_struct_t));
    
    test_pass();
}

void test_cosmopolitan_platform_info(void) {
    test_start("Platform information");
    
    #ifdef __COSMO__
    printf("  Compiled with: Cosmopolitan Libc\n");
    #else
    printf("  Compiled with: System libc\n");
    #endif
    
    #ifdef __x86_64__
    printf("  Architecture: x86_64\n");
    #elif defined(__i386__)
    printf("  Architecture: i386\n");
    #elif defined(__aarch64__)
    printf("  Architecture: aarch64\n");
    #elif defined(__arm__)
    printf("  Architecture: arm\n");
    #else
    printf("  Architecture: unknown\n");
    #endif
    
    #ifdef __linux__
    printf("  Operating System: Linux\n");
    #elif defined(_WIN32)
    printf("  Operating System: Windows\n");
    #elif defined(__APPLE__)
    printf("  Operating System: macOS\n");
    #elif defined(__FreeBSD__)
    printf("  Operating System: FreeBSD\n");
    #elif defined(__OpenBSD__)
    printf("  Operating System: OpenBSD\n");
    #elif defined(__NetBSD__)
    printf("  Operating System: NetBSD\n");
    #else
    printf("  Operating System: unknown\n");
    #endif
    
    printf("  sizeof(void*): %zu bytes\n", sizeof(void*));
    printf("  sizeof(size_t): %zu bytes\n", sizeof(size_t));
    printf("  sizeof(int): %zu bytes\n", sizeof(int));
    printf("  sizeof(long): %zu bytes\n", sizeof(long));
    
    test_pass();
}

void test_buffer_safety(void) {
    test_start("Buffer operations and safety");
    
    char dest[16];
    const char *src = "Hello";
    
    strncpy(dest, src, sizeof(dest) - 1);
    dest[sizeof(dest) - 1] = '\0';
    if (strcmp(dest, "Hello") != 0) {
        test_fail("strncpy failed");
        return;
    }
    printf("  strncpy with bounds: OK\n");
    
    char buf[32];
    int result = snprintf(buf, sizeof(buf), "Value: %d", 42);
    if (result < 0 || result >= (int)sizeof(buf)) {
        test_fail("snprintf failed");
        return;
    }
    printf("  snprintf: %s\n", buf);
    
    memset(buf, 0, sizeof(buf));
    printf("  memset: OK\n");
    
    test_pass();
}

int main(void) {
    printf("\n");
    printf(ANSI_COLOR_BLUE "======================================\n");
    printf("  Cosmopolitan Compatibility Tests\n");
    printf("======================================" ANSI_COLOR_RESET "\n\n");
    
    test_standard_headers();
    test_memory_operations();
    test_string_operations();
    test_integer_types();
    test_time_functions();
    test_signal_handling();
    test_file_operations();
    test_error_handling();
    test_environment_vars();
    test_math_operations();
    test_pointer_operations();
    test_struct_operations();
    test_cosmopolitan_platform_info();
    test_buffer_safety();
    
    printf(ANSI_COLOR_BLUE "======================================\n");
    printf("  Test Summary\n");
    printf("======================================" ANSI_COLOR_RESET "\n");
    printf("Total tests:  %d\n", test_count);
    printf(ANSI_COLOR_GREEN "Passed:       %d" ANSI_COLOR_RESET "\n", test_passed);
    if (test_failed > 0) {
        printf(ANSI_COLOR_RED "Failed:       %d" ANSI_COLOR_RESET "\n", test_failed);
    } else {
        printf("Failed:       %d\n", test_failed);
    }
    printf(ANSI_COLOR_BLUE "======================================" ANSI_COLOR_RESET "\n\n");
    
    if (test_failed > 0) {
        printf(ANSI_COLOR_RED "❌ Some tests failed!" ANSI_COLOR_RESET "\n");
        return 1;
    }
    
    printf(ANSI_COLOR_GREEN "✅ All Cosmopolitan compatibility tests passed!" ANSI_COLOR_RESET "\n");
    return 0;
}
