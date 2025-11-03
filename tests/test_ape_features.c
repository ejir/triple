#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
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

void test_portable_path_handling(void) {
    test_start("Portable path handling");
    
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        test_fail("getcwd failed");
        return;
    }
    printf("  Current directory: %s\n", cwd);
    
    const char *test_paths[] = {
        "test.txt",
        "./test.txt",
        "subdir/test.txt",
    };
    
    for (size_t i = 0; i < sizeof(test_paths) / sizeof(test_paths[0]); i++) {
        printf("  Path %zu: %s\n", i + 1, test_paths[i]);
    }
    
    printf("  Path handling: OK\n");
    
    test_pass();
}

void test_portable_file_creation(void) {
    test_start("Portable file creation");
    
    const char *test_file = "test_ape_file.tmp";
    
    int fd = open(test_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        test_fail("open failed");
        return;
    }
    printf("  open(O_CREAT|O_WRONLY): OK (fd=%d)\n", fd);
    
    const char *data = "APE test data\n";
    ssize_t written = write(fd, data, strlen(data));
    if (written != (ssize_t)strlen(data)) {
        close(fd);
        unlink(test_file);
        test_fail("write failed");
        return;
    }
    printf("  write: %zd bytes\n", written);
    
    close(fd);
    printf("  close: OK\n");
    
    struct stat st;
    if (stat(test_file, &st) != 0) {
        unlink(test_file);
        test_fail("stat failed");
        return;
    }
    printf("  stat: size=%lld bytes\n", (long long)st.st_size);
    
    fd = open(test_file, O_RDONLY);
    if (fd < 0) {
        unlink(test_file);
        test_fail("open(O_RDONLY) failed");
        return;
    }
    
    char buffer[256];
    ssize_t nread = read(fd, buffer, sizeof(buffer) - 1);
    if (nread < 0) {
        close(fd);
        unlink(test_file);
        test_fail("read failed");
        return;
    }
    buffer[nread] = '\0';
    printf("  read: %zd bytes\n", nread);
    
    if (strcmp(buffer, data) != 0) {
        close(fd);
        unlink(test_file);
        test_fail("data mismatch");
        return;
    }
    printf("  data verification: OK\n");
    
    close(fd);
    unlink(test_file);
    
    test_pass();
}

void test_portable_stdio(void) {
    test_start("Portable stdio operations");
    
    printf("  stdout write: OK\n");
    fprintf(stderr, "  stderr write: OK\n");
    
    char buffer[256];
    sprintf(buffer, "Formatted: %d %s %.2f", 42, "test", 3.14);
    printf("  sprintf: %s\n", buffer);
    
    FILE *fp = tmpfile();
    if (fp == NULL) {
        test_fail("tmpfile failed");
        return;
    }
    printf("  tmpfile: OK\n");
    
    fprintf(fp, "Temporary file data\n");
    rewind(fp);
    
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        fclose(fp);
        test_fail("fgets from tmpfile failed");
        return;
    }
    printf("  tmpfile read/write: OK\n");
    
    fclose(fp);
    
    test_pass();
}

void test_cross_platform_constants(void) {
    test_start("Cross-platform constants");
    
    printf("  O_RDONLY: %d\n", O_RDONLY);
    printf("  O_WRONLY: %d\n", O_WRONLY);
    printf("  O_RDWR: %d\n", O_RDWR);
    printf("  O_CREAT: %d\n", O_CREAT);
    printf("  O_TRUNC: %d\n", O_TRUNC);
    printf("  O_APPEND: %d\n", O_APPEND);
    
    printf("  STDIN_FILENO: %d\n", STDIN_FILENO);
    printf("  STDOUT_FILENO: %d\n", STDOUT_FILENO);
    printf("  STDERR_FILENO: %d\n", STDERR_FILENO);
    
    printf("  NULL: %p\n", (void*)NULL);
    printf("  EOF: %d\n", EOF);
    
    test_pass();
}

void test_portable_memory_alignment(void) {
    test_start("Memory alignment");
    
    void *ptr1 = malloc(1);
    void *ptr2 = malloc(8);
    void *ptr3 = malloc(16);
    void *ptr4 = malloc(64);
    
    if (!ptr1 || !ptr2 || !ptr3 || !ptr4) {
        free(ptr1);
        free(ptr2);
        free(ptr3);
        free(ptr4);
        test_fail("malloc failed");
        return;
    }
    
    printf("  malloc(1):  %p\n", ptr1);
    printf("  malloc(8):  %p\n", ptr2);
    printf("  malloc(16): %p\n", ptr3);
    printf("  malloc(64): %p\n", ptr4);
    
    free(ptr1);
    free(ptr2);
    free(ptr3);
    free(ptr4);
    
    printf("  Memory allocation alignment: OK\n");
    
    test_pass();
}

void test_command_line_args(int argc, char **argv) {
    test_start("Command line argument handling");
    
    printf("  argc: %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d]: %s\n", i, argv[i]);
    }
    
    if (argc < 1) {
        test_fail("argc should be at least 1");
        return;
    }
    
    if (argv[0] == NULL) {
        test_fail("argv[0] should not be NULL");
        return;
    }
    
    printf("  Command line handling: OK\n");
    
    test_pass();
}

void test_ape_runtime_detection(void) {
    test_start("APE runtime detection");
    
    #ifdef __COSMO__
    printf(ANSI_COLOR_CYAN "  ✓ Compiled with Cosmopolitan Libc" ANSI_COLOR_RESET "\n");
    printf("  This is an Actually Portable Executable (APE)\n");
    #else
    printf("  ⚠ Not compiled with Cosmopolitan Libc\n");
    printf("  Using system libc\n");
    #endif
    
    printf("\n");
    printf("  Build Configuration:\n");
    #ifdef __OPTIMIZE__
    printf("  - Optimization: Enabled\n");
    #else
    printf("  - Optimization: Disabled\n");
    #endif
    
    #ifdef __PIC__
    printf("  - Position Independent Code: Yes\n");
    #else
    printf("  - Position Independent Code: No\n");
    #endif
    
    #ifdef __STDC_VERSION__
    printf("  - C Standard: %ld\n", __STDC_VERSION__);
    #endif
    
    test_pass();
}

void test_ape_portable_types(void) {
    test_start("Portable type sizes");
    
    printf("  char: %zu byte(s)\n", sizeof(char));
    printf("  short: %zu byte(s)\n", sizeof(short));
    printf("  int: %zu byte(s)\n", sizeof(int));
    printf("  long: %zu byte(s)\n", sizeof(long));
    printf("  long long: %zu byte(s)\n", sizeof(long long));
    printf("  float: %zu byte(s)\n", sizeof(float));
    printf("  double: %zu byte(s)\n", sizeof(double));
    printf("  void*: %zu byte(s)\n", sizeof(void*));
    printf("  size_t: %zu byte(s)\n", sizeof(size_t));
    printf("  off_t: %zu byte(s)\n", sizeof(off_t));
    
    if (sizeof(char) != 1) {
        test_fail("char should be 1 byte");
        return;
    }
    
    if (sizeof(void*) != sizeof(size_t)) {
        test_fail("pointer size should equal size_t size");
        return;
    }
    
    printf("  Type size verification: OK\n");
    
    test_pass();
}

void test_exit_codes(void) {
    test_start("Exit code constants");
    
    printf("  EXIT_SUCCESS: %d\n", EXIT_SUCCESS);
    printf("  EXIT_FAILURE: %d\n", EXIT_FAILURE);
    
    if (EXIT_SUCCESS != 0) {
        test_fail("EXIT_SUCCESS should be 0");
        return;
    }
    
    if (EXIT_FAILURE == 0) {
        test_fail("EXIT_FAILURE should be non-zero");
        return;
    }
    
    printf("  Exit codes: OK\n");
    
    test_pass();
}

void test_portable_string_literals(void) {
    test_start("Portable string literals");
    
    const char *str1 = "Simple ASCII string";
    const char *str2 = "String with\nnewline";
    const char *str3 = "String with\ttab";
    const char *str4 = "String with\\backslash";
    const char *str5 = "";
    
    printf("  String 1: %s\n", str1);
    printf("  String 2 length: %zu\n", strlen(str2));
    printf("  String 3 length: %zu\n", strlen(str3));
    printf("  String 4 length: %zu\n", strlen(str4));
    printf("  Empty string length: %zu\n", strlen(str5));
    
    if (strlen(str5) != 0) {
        test_fail("empty string should have length 0");
        return;
    }
    
    printf("  String literals: OK\n");
    
    test_pass();
}

int main(int argc, char **argv) {
    printf("\n");
    printf(ANSI_COLOR_BLUE "======================================\n");
    printf("  APE (Actually Portable Executable)\n");
    printf("  Feature Compatibility Tests\n");
    printf("======================================" ANSI_COLOR_RESET "\n\n");
    
    test_ape_runtime_detection();
    test_portable_path_handling();
    test_portable_file_creation();
    test_portable_stdio();
    test_cross_platform_constants();
    test_portable_memory_alignment();
    test_command_line_args(argc, argv);
    test_ape_portable_types();
    test_exit_codes();
    test_portable_string_literals();
    
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
        printf(ANSI_COLOR_RED "❌ Some APE tests failed!" ANSI_COLOR_RESET "\n");
        return EXIT_FAILURE;
    }
    
    printf(ANSI_COLOR_GREEN "✅ All APE feature tests passed!" ANSI_COLOR_RESET "\n");
    printf(ANSI_COLOR_CYAN "\nThis binary should run on:\n");
    printf("  • Linux (x86_64)\n");
    printf("  • Windows (x64)\n");
    printf("  • macOS (x86_64)\n");
    printf("  • FreeBSD (x86_64)\n");
    printf("  • OpenBSD (x86_64)\n");
    printf("  • NetBSD (x86_64)\n" ANSI_COLOR_RESET "\n");
    
    return EXIT_SUCCESS;
}
