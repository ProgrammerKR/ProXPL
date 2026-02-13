#include <stdio.h>

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f) return 1;
    int balance = 0;
    int line = 1;
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (c == '{') {
            balance++;
        } else if (c == '}') {
            balance--;
            if (balance < 0) {
                printf("Negative balance at line %d : %d\n", line, balance);
            }
        } else if (c == '\n') {
            line++;
        }
    }
    printf("Final balance: %d at line %d\n", balance, line);
    fclose(f);
    return 0;
}
