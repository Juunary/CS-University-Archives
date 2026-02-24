/*
Concatenate strings with dynamic allocation.
- Read two strings, store them in a pointer array.
- Concatenate them into a third dynamically allocated string (strcat).
- Print the two originals and the concatenated result.
*/
#include <stdio.h>
#include <malloc.h>
#include <string.h>

void main() {
    char* p[3];
    char imsi[100];
    int i, size;

    for (i = 0; i < 2; i++) {
        printf("String %d: ", i + 1);
        gets(imsi);
        size = strlen(imsi);
        p[i] = (char*)malloc(sizeof(char) * (size + 1));
        strcpy(p[i], imsi);
    }

    // Concatenate two strings
    size = strlen(p[0]) + strlen(p[1]);
    p[2] = (char*)malloc(sizeof(char) * (size + 1));
    strcpy(p[2], p[0]);
    strcat(p[2], p[1]);

    printf("\nOutput original strings and concatenated string (pointers)\n");
    for (i = 0; i < 3; i++) {
        printf("%d : %s\n", i + 1, p[i]);
    }

    for (i = 0; i < 3; i++) {
        free(p[i]);
    }
}