# ProXPL C Implementation Makefile
# Complete build system for the C-based ProXPL interpreter

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -I../include
LDFLAGS = -lm
TARGET = prox
SRCDIR = .
INCDIR = ../include
OBJDIR = obj

# All source files
SOURCES = main.c \
          lexer/scanner.c \
          parser/parser.c \
          parser/ast.c \
          parser/type_checker.c \
          runtime/vm.c \
          runtime/chunk.c \
          runtime/compiler.c \
          runtime/value.c \
          runtime/object.c \
          runtime/memory.c \
          runtime/debug.c \
          stdlib/stdlib_core.c \
          stdlib/io_native.c \
          stdlib/math_native.c \
          stdlib/string_native.c \
          stdlib/convert_native.c \
          stdlib/system_native.c

# Object files
OBJECTS = $(patsubst %.c,$(OBJDIR)/%.o,$(SOURCES))

# Default target
all: $(TARGET)

# Create object directory structure
$(OBJDIR):
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/lexer
	@mkdir -p $(OBJDIR)/parser
	@mkdir -p $(OBJDIR)/runtime
	@mkdir -p $(OBJDIR)/stdlib

# Link the executable
$(TARGET): $(OBJDIR) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(TARGET) $(TARGET).exe
	@echo "Clean complete"

# Rebuild from scratch
rebuild: clean all

# Run the interpreter (REPL mode)
repl: $(TARGET)
	./$(TARGET)

# Run a test file
test: $(TARGET)
	./$(TARGET) ../examples/hello.prox

# Show help
help:
	@echo "ProXPL C Implementation - Makefile"
	@echo "Targets:"
	@echo "  all      - Build the interpreter (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  rebuild  - Clean and rebuild"
	@echo "  repl     - Run in REPL mode"
	@echo "  test     - Run test example"
	@echo "  help     - Show this help"

.PHONY: all clean rebuild repl test help
