#include <stdio.h>
#include <stdbool.h>

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f) return 1;

    int balance = 0;
    int line = 1;
    int c;
    bool in_string = false;
    bool in_char = false;
    bool in_line_comment = false;
    bool in_block_comment = false;
    int prev = 0;

    while ((c = fgetc(f)) != EOF) {
        if (in_line_comment) {
            if (c == '\n') in_line_comment = false;
        } else if (in_block_comment) {
            if (prev == '*' && c == '/') in_block_comment = false;
        } else if (in_string) {
            if (c == '"' && prev != '\\') in_string = false;
        } else if (in_char) {
            if (c == '\'' && prev != '\\') in_char = false;
        } else {
            if (prev == '/' && c == '/') {
                in_line_comment = true;
            } else if (prev == '/' && c == '*') {
                in_block_comment = true;
            } else if (c == '"') {
                in_string = true;
            } else if (c == '\'') {
                in_char = true;
            } else if (c == '{') {
                balance++;
                printf("Line %d: { (balance: %d)\n", line, balance);
            } else if (c == '}') {
                balance--;
                printf("Line %d: } (balance: %d)\n", line, balance);
                if (balance < 0) {
                    printf("NEGATIVE BALANCE at line %d\n", line);
                }
            }
        }
        
        if (c == '\n') line++;
        prev = c;
    }

    printf("Final balance: %d\n", balance);
    fclose(f);
    return 0;
}
