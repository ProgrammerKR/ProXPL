// --------------------------------------------------
//   Project: ProX Programming Language (ProXPL)
//   Author:  ProgrammerKR
//   Created: 2025-12-16
//   Copyright © 2025. ProXentix India Pvt. Ltd.  All rights reserved.
// --------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/error_report.h"

static int min3(int a, int b, int c) {
    int m = a < b ? a : b;
    return m < c ? m : c;
}

static int levenshteinDistance(const char* s1, const char* s2) {
    int len1 = (int)strlen(s1);
    int len2 = (int)strlen(s2);
    if (len1 == 0) return len2;
    if (len2 == 0) return len1;

    int* v0 = (int*)malloc((len2 + 1) * sizeof(int));
    int* v1 = (int*)malloc((len2 + 1) * sizeof(int));

    for (int i = 0; i <= len2; i++) v0[i] = i;

    for (int i = 0; i < len1; i++) {
        v1[0] = i + 1;
        for (int j = 0; j < len2; j++) {
            int cost = (s1[i] == s2[j]) ? 0 : 1;
            v1[j + 1] = min3(v1[j] + 1, v0[j + 1] + 1, v0[j] + cost);
        }
        for (int j = 0; j <= len2; j++) v0[j] = v1[j];
    }

    int dist = v1[len2];
    free(v0);
    free(v1);
    return dist;
}

const char* findTypoSuggestion(const char* target, const char** candidates, int candidateCount) {
    if (!target || !candidates || candidateCount == 0) return NULL;
    const char* best = NULL;
    int bestDist = 4; // threshold

    for (int i = 0; i < candidateCount; i++) {
        if (candidates[i] == NULL) continue;
        int dist = levenshteinDistance(target, candidates[i]);
        if (dist < bestDist) {
            bestDist = dist;
            best = candidates[i];
        }
    }
    return best;
}

static void printLineSnippet(const char* source, int targetLine, int col, int length, const char* suggestion) {
    if (!source) return;
    int currentLine = 1;
    const char* lineStart = source;

    while (*lineStart != '\0') {
        const char* lineEnd = lineStart;
        while (*lineEnd != '\n' && *lineEnd != '\0') lineEnd++;

        if (currentLine == targetLine) {
            int lineLen = (int)(lineEnd - lineStart);
            fprintf(stderr, "%4d | %.*s\n", targetLine, lineLen, lineStart);
            fprintf(stderr, "     | ");
            int indent = (col > 1) ? col - 1 : 0;
            for (int i = 0; i < indent; i++) fprintf(stderr, " ");
            int caretLen = (length > 0) ? length : 1;
            for (int i = 0; i < caretLen; i++) fprintf(stderr, "^");
            if (suggestion != NULL) {
                fprintf(stderr, " help: did you mean '%s'?", suggestion);
            }
            fprintf(stderr, "\n");
            return;
        }

        if (*lineEnd == '\n') {
            currentLine++;
            lineStart = lineEnd + 1;
        } else {
            break;
        }
    }
}

void reportDiagnostic(const char* code, const char* source, int line, int col, int length, const char* message, const char* suggestion) {
    fprintf(stderr, "\033[1;31merror[%s]\033[0m: %s\n", code ? code : "E0001", message);
    fprintf(stderr, "  --> line %d:%d\n", line, col > 0 ? col : 1);
    fprintf(stderr, "     |\n");
    if (source != NULL) {
        printLineSnippet(source, line, col, length, suggestion);
    }
    fprintf(stderr, "     |\n");
}

void reportCompileError(const char* source, Token token, const char* message) {
    reportDiagnostic(ERR_SYNTAX, source, token.line, token.column, token.length, message, NULL);
}

void reportRuntimeError(const char* source, int line, const char* message) {
    fprintf(stderr, "\033[1;31mruntime error[%s]\033[0m: %s\n", ERR_RUNTIME, message);
    if (source != NULL) {
        fprintf(stderr, "  --> line %d:1\n", line);
        fprintf(stderr, "     |\n");
        printLineSnippet(source, line, 1, 1, NULL);
        fprintf(stderr, "     |\n");
    } else {
        fprintf(stderr, "[line %d]\n", line);
    }
}
