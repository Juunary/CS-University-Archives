#include <stdio.h>
#include <malloc.h>

void main() {
    char **p = (char**)malloc(sizeof(char*) * 2); 
    char imsi1[100], imsi2[100];
    int i, size1, size2;
    int is_equal = 1; 
    printf("Enter the first string: ");
    scanf("%s", imsi1); 
    size1 = strlen(imsi1);
    p[0] = (char*)malloc(sizeof(char) * (size1 + 1));
    strcpy(p[0], imsi1);
    p[0][size1] = '\0'; 
    printf("Enter the second string: ");
    scanf("%s", imsi2);
    size2 = strlen(imsi2);
    p[1] = (char*)malloc(sizeof(char) * (size2 + 1));
    strcpy(p[1], imsi2);
    p[1][size2] = '\0'; 
    printf("First input string: %s\n", p[0]);
    printf("Second input string: %s\n", p[1]);

    if (size1 != size2) {
        is_equal = 0;
    } else {
        for (i = 0; i < size1; i++) {
            if (p[0][i] >= 'A' && p[0][i] <= 'Z') {
                p[0][i] += 32;
            }
            if (p[1][i] >= 'A' && p[1][i] <= 'Z') {
                p[1][i] += 32;
            }
            if (p[0][i] != p[1][i]) {
                is_equal = 0; 
                break;
            }
        }
    }
    if (is_equal) {
        printf("True\n");
    } else {
        printf("False\n");
    }

}
