#include <stdio.h>
#include <stdlib.h>

#ifndef MAX_PROCESSES
#define MAX_PROCESSES 64
#endif

int main(void) {
    const char *path = "./temp.txt";
    FILE *f = fopen(path, "w");
    if (!f) {
        perror("fopen");
        return 1;
    }

    for (int i = 0; i < MAX_PROCESSES * 2; i++) {
        if (fprintf(f, "%d\n", i + 1) < 0) {
            perror("fprintf");
            fclose(f);
            return 1;
        }
    }
    if (fclose(f) != 0) {
        perror("fclose");
        return 1;
    }
    printf("Generated %d numbers into %s\n", MAX_PROCESSES * 2, path);
    return 0;
}
