#ifndef PROXPL_FORMATTER_H
#define PROXPL_FORMATTER_H

#include <stdbool.h>

typedef struct {
    int indentSize;
    bool useTabs;
    int maxLineLength;
    bool sameLineBrace;
    bool trailingComma;
    bool sortImports;
} FormatConfig;

// Loads format configuration from .proxfmt.pxcf if present, or defaults
FormatConfig loadFormatConfig(const char* configPath);

// Formats source code string according to config, returns newly allocated formatted string
char* formatSource(const char* source, const FormatConfig* config);

// Formats a file or directory recursively. If checkOnly is true, returns whether file needs formatting.
int formatPath(const char* path, bool checkOnly, const FormatConfig* config);

#endif // PROXPL_FORMATTER_H
