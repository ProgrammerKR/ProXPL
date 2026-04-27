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
          compiler/lexer/scanner.c \
          compiler/parser/ast.c \
          compiler/parser/parser.c \
          compiler/bytecode_gen.c \
          compiler/importer.c \
          compiler/ir.c \
          compiler/ir_gen.c \
          compiler/ir_opt.c \
          compiler/optimizer.c \
          compiler/transpiler_ui.c \
          compiler/type_checker.c \
          prm/builder.c \
          prm/manifest.c \
          prm/commands/cmd_core.c \
          protos/vm_register.c \
          proxpl_api.c \
          runtime/chunk.c \
          runtime/compiler.c \
          runtime/debug.c \
          runtime/ffi_bridge.c \
          runtime/gc.c \
          runtime/llvm_runtime.c \
          runtime/memory.c \
          runtime/object.c \
          runtime/scheduler.c \
          runtime/supervisor.c \
          runtime/table.c \
          runtime/value.c \
          runtime/vm.c \
          runtime/vm_helpers.c \
          stdlib/collections_native.c \
          stdlib/convert_native.c \
          stdlib/core_native.c \
          stdlib/fs_native.c \
          stdlib/gc_native.c \
          stdlib/hash_native.c \
          stdlib/io_native.c \
          stdlib/json_native.c \
          stdlib/math_native.c \
          stdlib/net_native.c \
          stdlib/os_native.c \
          stdlib/reflect_native.c \
          stdlib/stdlib_core.c \
          stdlib/string_native.c \
          stdlib/system_native.c \
          stdlib/sys_native.c \
          stdlib/time_native.c \
          utils/error_report.c \
          utils/md5.c \
          utils/pxcf.c \
          utils/sha256.c \
          utils/file_utils.c \
          vm/bytecode.c \
          vm/disasm.c \
          vm/instr_handlers_template.c \
          vm/vm_core_opt.c \
          vm/vm_dispatch.c \
          vm/vm_v2.c

# Object files
OBJECTS = $(patsubst %.c,$(OBJDIR)/%.o,$(SOURCES))

# Default target
all: $(TARGET)

# Create object directory structure
$(OBJDIR):
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OBJDIR)/compiler/lexer
	@mkdir -p $(OBJDIR)/compiler/parser
	@mkdir -p $(OBJDIR)/prm/commands
	@mkdir -p $(OBJDIR)/protos
	@mkdir -p $(OBJDIR)/runtime
	@mkdir -p $(OBJDIR)/stdlib
	@mkdir -p $(OBJDIR)/utils
	@mkdir -p $(OBJDIR)/vm

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
