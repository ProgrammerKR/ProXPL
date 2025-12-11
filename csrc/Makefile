# ProXPL C Implementation Makefile
# Build system for the C-based ProXPL interpreter

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -I../include
LDFLAGS = 
TARGET = prox
SRCDIR = .
INCDIR = ../include
OBJDIR = obj

# Source files (to be expanded as implementation progresses)
SOURCES = main.c

# Object files
OBJECTS = $(SOURCES:%.c=$(OBJDIR)/%.o)

# Default target
all: $(TARGET)

# Create object directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Link the executable
$(TARGET): $(OBJDIR) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(TARGET)
	@echo "Clean complete"

# Rebuild from scratch
rebuild: clean all

# Run the interpreter
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean rebuild run
