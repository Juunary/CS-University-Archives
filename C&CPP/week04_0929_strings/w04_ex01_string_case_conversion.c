

#include <stdio.h>
#include <malloc.h>
#include <string.h>

void main () {
    char ** p = (char**)malloc(sizeof(char*) * 1);
    char imsi[100];
    int i, size;
    printf("Enter a string: ");
    gets(imsi);
    size = strlen(imsi);
    p[0] = (char*)malloc(sizeof(char) * (size + 1));
    strcpy(p[0], imsi);
    printf("Input string: %s\n", p[0]);
    printf("Uppercase: ");
    for (i = 0; i < size; i++) {
        if (p[0][i] >= 'a' && p[0][i] <= 'z') {
            printf("%c", p[0][i] - 32);
        } else {
            printf("%c", p[0][i]);
        }
    }
    printf("\n");
    printf("Lowercase: ");
    for (i = 0; i < size; i++) {
        if (p[0][i] >= 'A' && p[0][i] <= 'Z') {
            printf("%c", p[0][i] + 32);
        } else {
            printf("%c", p[0][i]);
        }
    }
    printf("\n");
    free(p[0]);
    free(p);
}