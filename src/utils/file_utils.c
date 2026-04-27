// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.

#include <stdio.h>
#include <stdlib.h>
#include "../include/file_utils.h"

char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    long ftellSize = ftell(file);
    if (ftellSize < 0) {
        fprintf(stderr, "Could not determine size of file \"%s\".\n", path);
        fclose(file);
        return NULL;
    }
    size_t fileSize = (size_t)ftellSize;
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[bytesRead] = '\0';
    fclose(file);
    return buffer;
}
