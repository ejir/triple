.PHONY: all clean run help dev test

# Build mode: cosmo (default) or gcc (fallback for development)
BUILD_MODE ?= cosmo

ifeq ($(BUILD_MODE),gcc)
	# Standard GCC build for development/testing
	CC = gcc
	CFLAGS = -O2 -Wall -Wextra -std=c11 -Isrc -Ithird_party/sqlite3
	LDFLAGS = -lpthread -ldl -lm
	TARGET = app
else
	# Cosmopolitan build (default)
	COSMO_DIR ?= /opt/cosmo
	CC = $(COSMO_DIR)/bin/cosmocc
	AR = $(COSMO_DIR)/bin/cosmoar
	CFLAGS = -O2 -Wall -Wextra -std=c11 -Isrc -Ithird_party/sqlite3
	LDFLAGS = -static
	TARGET = app.com
endif

SRC_DIR = src
OBJ_DIR = obj
SQLITE3_DIR = third_party/sqlite3
TEST_DIR = tests

SOURCES = $(wildcard $(SRC_DIR)/*.c)
SQLITE3_SRC = $(SQLITE3_DIR)/sqlite3.c
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
SQLITE3_OBJ = $(OBJ_DIR)/sqlite3.o

TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(OBJ_DIR)/%)

all: $(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(SQLITE3_OBJ): $(SQLITE3_SRC) | $(OBJ_DIR)
	@echo "Compiling SQLite3..."
	$(CC) $(CFLAGS) -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -c $< -o $@

$(TARGET): $(OBJECTS) $(SQLITE3_OBJ)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) $(SQLITE3_OBJ) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

$(OBJ_DIR)/test_sqlite3: $(TEST_DIR)/test_sqlite3.c $(SQLITE3_OBJ) | $(OBJ_DIR)
	@echo "Compiling test $<..."
	$(CC) $(CFLAGS) $< $(SQLITE3_OBJ) $(LDFLAGS) -o $@

$(OBJ_DIR)/test_db: $(TEST_DIR)/test_db.c $(OBJ_DIR)/db.o $(SQLITE3_OBJ) | $(OBJ_DIR)
	@echo "Compiling test $<..."
	$(CC) $(CFLAGS) $< $(OBJ_DIR)/db.o $(SQLITE3_OBJ) $(LDFLAGS) -o $@

test: $(TEST_BINS)
	@echo "Running tests..."
	@for test in $(TEST_BINS); do \
		echo "Running $$test..."; \
		$$test || exit 1; \
	done
	@echo "All tests passed!"

clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(OBJ_DIR) $(TARGET) app app.com *.db
	@echo "Clean complete"

run: $(TARGET)
	./$(TARGET)

dev:
	@echo "Building in development mode with GCC..."
	$(MAKE) BUILD_MODE=gcc

help:
	@echo "Cosmopolitan Build System"
	@echo ""
	@echo "Usage:"
	@echo "  make              - Build with Cosmopolitan (default)"
	@echo "  make dev          - Build with GCC for development"
	@echo "  make clean        - Remove build artifacts"
	@echo "  make run          - Build and run the application"
	@echo "  make test         - Build and run tests"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  BUILD_MODE=cosmo  - Use Cosmopolitan toolchain (default)"
	@echo "  BUILD_MODE=gcc    - Use standard GCC compiler"
	@echo "  COSMO_DIR         - Path to Cosmopolitan (default: /opt/cosmo)"
	@echo ""
	@echo "Examples:"
	@echo "  make                        - Build APE with Cosmopolitan"
	@echo "  make BUILD_MODE=gcc         - Build with GCC"
	@echo "  make dev                    - Quick development build"
	@echo "  make test                   - Run SQLite3 tests"
	@echo ""
	@echo "Output:"
	@echo "  app.com (cosmo)  - Actually Portable Executable"
	@echo "  app (gcc)        - Standard Linux executable"
