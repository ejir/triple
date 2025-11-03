.PHONY: all clean run help dev

# Build mode: cosmo (default) or gcc (fallback for development)
BUILD_MODE ?= cosmo

ifeq ($(BUILD_MODE),gcc)
    # Standard GCC build for development/testing
    CC = gcc
    CFLAGS = -O2 -Wall -Wextra -std=c11 -Isrc
    LDFLAGS = 
    TARGET = app
else
    # Cosmopolitan build (default)
    COSMO_DIR ?= /opt/cosmo
    CC = $(COSMO_DIR)/bin/cosmocc
    AR = $(COSMO_DIR)/bin/cosmoar
    CFLAGS = -O2 -Wall -Wextra -std=c11 -Isrc
    LDFLAGS = -static
    TARGET = app.com
endif

SRC_DIR = src
OBJ_DIR = obj

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(OBJ_DIR) $(TARGET) app app.com
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
	@echo ""
	@echo "Output:"
	@echo "  app.com (cosmo)  - Actually Portable Executable"
	@echo "  app (gcc)        - Standard Linux executable"
