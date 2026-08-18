#include "../include/formatter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#define PATH_SEP '\\'
#else
#include <dirent.h>
#include <unistd.h>
#define PATH_SEP '/'
#endif

FormatConfig loadFormatConfig(const char* configPath) {
    FormatConfig cfg = {
        .indentSize = 4,
        .useTabs = false,
        .maxLineLength = 100,
        .sameLineBrace = true,
        .trailingComma = false,
        .sortImports = true
    };

    FILE* f = fopen(configPath ? configPath : ".proxfmt.pxcf", "r");
    if (!f) return cfg;

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char key[64], val[64];
        if (sscanf(line, " %63[^= ] = %63s", key, val) == 2) {
            if (strcmp(key, "indent_size") == 0) cfg.indentSize = atoi(val);
            else if (strcmp(key, "use_tabs") == 0) cfg.useTabs = (strcmp(val, "true") == 0);
            else if (strcmp(key, "max_line_length") == 0) cfg.maxLineLength = atoi(val);
            else if (strcmp(key, "brace_style") == 0) cfg.sameLineBrace = (strcmp(val, "same_line") == 0);
            else if (strcmp(key, "trailing_comma") == 0) cfg.trailingComma = (strcmp(val, "true") == 0);
            else if (strcmp(key, "sort_imports") == 0) cfg.sortImports = (strcmp(val, "true") == 0);
        }
    }
    fclose(f);
    return cfg;
}

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) return NULL;
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(fileSize + 1);
    if (!buffer) { fclose(file); return NULL; }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    buffer[bytesRead] = '\0';
    fclose(file);
    return buffer;
}

static bool writeFile(const char* path, const char* content) {
    FILE* file = fopen(path, "wb");
    if (!file) return false;
    fputs(content, file);
    fclose(file);
    return true;
}

char* formatSource(const char* source, const FormatConfig* config) {
    if (!source) return NULL;
    FormatConfig cfg = config ? *config : loadFormatConfig(NULL);

    size_t srcLen = strlen(source);
    size_t outCap = srcLen * 2 + 1024;
    char* out = (char*)malloc(outCap);
    size_t outIdx = 0;

    int indentLevel = 0;
    const char* p = source;
    bool atLineStart = true;
    bool inString = false;
    char strQuote = 0;
    bool inTemplate = false;

    while (*p) {
        if (inString) {
            out[outIdx++] = *p;
            if (*p == '\\' && *(p + 1)) {
                p++;
                out[outIdx++] = *p;
            } else if (*p == strQuote) {
                inString = false;
            }
            p++;
            atLineStart = false;
            continue;
        }

        if (inTemplate) {
            out[outIdx++] = *p;
            if (*p == '\\' && *(p + 1)) {
                p++;
                out[outIdx++] = *p;
            } else if (*p == '`') {
                inTemplate = false;
            }
            p++;
            atLineStart = false;
            continue;
        }

        if (*p == '"' || *p == '\'') {
            inString = true;
            strQuote = *p;
            out[outIdx++] = *p++;
            atLineStart = false;
            continue;
        }

        if (*p == '`') {
            inTemplate = true;
            out[outIdx++] = *p++;
            atLineStart = false;
            continue;
        }

        // Line comments
        if (*p == '/' && *(p + 1) == '/') {
            while (*p && *p != '\n') {
                out[outIdx++] = *p++;
            }
            continue;
        }

        // Leading whitespace on new line
        if (atLineStart) {
            while (*p == ' ' || *p == '\t') p++;
            if (*p == '\n') {
                out[outIdx++] = '\n';
                p++;
                continue;
            }
            if (*p == '\0') break;

            if (*p == '}') {
                if (indentLevel > 0) indentLevel--;
            }

            int spaces = indentLevel * cfg.indentSize;
            if (cfg.useTabs) {
                for (int i = 0; i < indentLevel; i++) out[outIdx++] = '\t';
            } else {
                for (int i = 0; i < spaces; i++) out[outIdx++] = ' ';
            }
            atLineStart = false;
        }

        if (*p == '{') {
            out[outIdx++] = '{';
            indentLevel++;
            p++;
            continue;
        }

        if (*p == '}') {
            out[outIdx++] = '}';
            p++;
            continue;
        }

        if (*p == '\n') {
            out[outIdx++] = '\n';
            atLineStart = true;
            p++;
            continue;
        }

        out[outIdx++] = *p++;
    }

    out[outIdx] = '\0';
    return out;
}

static bool isDirectory(const char* path) {
    struct stat s;
    if (stat(path, &s) == 0) {
        return (s.st_mode & S_IFDIR) != 0;
    }
    return false;
}

static bool hasExtension(const char* path, const char* ext) {
    const char* dot = strrchr(path, '.');
    return dot && strcmp(dot, ext) == 0;
}

int formatPath(const char* path, bool checkOnly, const FormatConfig* config) {
    if (!path) return 0;

    if (!isDirectory(path)) {
        if (!hasExtension(path, ".prox") && !hasExtension(path, ".px")) {
            return 0;
        }
        char* source = readFile(path);
        if (!source) return 0;
        char* formatted = formatSource(source, config);
        bool modified = (strcmp(source, formatted) != 0);
        if (modified) {
            if (checkOnly) {
                printf("[FMT CHECK] %s needs formatting.\n", path);
            } else {
                writeFile(path, formatted);
                printf("[FMT] Formatted %s\n", path);
            }
        }
        free(source);
        free(formatted);
        return modified ? 1 : 0;
    }

#ifdef _WIN32
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s\\*", path);
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath, &findData);
    int changes = 0;
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
                continue;
            char subPath[MAX_PATH];
            snprintf(subPath, sizeof(subPath), "%s\\%s", path, findData.cFileName);
            changes += formatPath(subPath, checkOnly, config);
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
    return changes;
#else
    DIR* dir = opendir(path);
    if (!dir) return 0;
    struct dirent* entry;
    int changes = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char subPath[1024];
        snprintf(subPath, sizeof(subPath), "%s/%s", path, entry->d_name);
        changes += formatPath(subPath, checkOnly, config);
    }
    closedir(dir);
    return changes;
#endif
}
