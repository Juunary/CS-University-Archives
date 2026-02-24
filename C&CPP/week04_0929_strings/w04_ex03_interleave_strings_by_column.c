#include <stdio.h>
#include <malloc.h>
#include <string.h>

void main () {
    char imsi[100];           
    int i, size;              

    printf("Number of strings to input: ");
    scanf("%d", &size);
    

    char **p = (char**)malloc(sizeof(char*) * size); 

    for ( i = 0; i < size; i++ ) {
        printf("String %d: ", i + 1);
        scanf("%s", imsi); // Input string
        p[i] = (char*)malloc(sizeof(char) * (strlen(imsi) + 1));
        strcpy(p[i], imsi);
        p[i][strlen(imsi)] = '\0';
    }

    int maxlen = 0;
    for (i = 0; i < size; ++i) {
        int l = (int)strlen(p[i]);
        if (l > maxlen) maxlen = l;
    }

    for (int col = 0; col < maxlen; ++col) {
        for (i = 0; i < size; ++i) {
            if (col < (int)strlen(p[i])) {
                printf("%c", p[i][col]);
            }
        }
    }
    printf("\n");


    for ( i = 0; i < size; i++ ) {
        free(p[i]);
    }
    free(p);
}
