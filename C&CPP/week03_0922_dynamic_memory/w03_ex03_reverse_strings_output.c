/*
Print input strings in reverse order (dynamic allocation + pointers).
- Read 3 strings, store each in dynamically allocated memory,
  then print them in reverse input order.
*/
#include <stdio.h>
#include <malloc.h>
#include <string.h>

void main () {
    char* p[3];
    char imsi[100];
    int i, size;

    for( i = 0; i < 3; i++ ) {
        printf("String %d: ", i + 1);
        gets(imsi);
        size = strlen(imsi);
        p[i] = (char*)malloc(sizeof(char) * (size + 1));
        strcpy(p[i], imsi);
    }

    printf("\nOutput strings in reverse order (pointers)\n");
    for( i = 2; i >= 0; i-- ) {
        printf("%d : %s\n", i + 1, p[i]);
    }

    for( i = 0; i < 3; i++ ) {
        free(p[i]);
    }
}