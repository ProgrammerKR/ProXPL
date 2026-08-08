# ProXPL C Implementation Makefile
# DEPRECATED: Use CMake for the primary build system.
# This Makefile is retained for reference only and may not build successfully.
# See BUILD_GUIDE.md or CMakeLists.txt for the canonical build instructions.

CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -Wpedantic -std=c99 -O2 -Iinclude
LDFLAGS = -lm -lmimalloc
TARGET = proxpl
SRCDIR = src
INCDIR = include
OBJDIR = build/obj

# Default target
all: help

help:
	@echo "ProXPL C Implementation - Makefile (DEPRECATED)"
	@echo "Please use CMake instead:"
	@echo "  mkdir build && cd build"
	@echo "  cmake .. && make"
	@echo ""
	@echo "Or on Windows:"
	@echo "  cmake -G 'Visual Studio 16 2019' .."

.PHONY: all help
