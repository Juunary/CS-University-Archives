

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void reverse_string(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int main() {
    char **strings;
    int n;

    printf("Number of strings to input: ");
    scanf("%d", &n);
    getchar();

    strings = (char **)malloc(n * sizeof(char *));
    for (int i = 0; i < n; i++) {
        strings[i] = (char *)malloc(100 * sizeof(char));
    }

    for (int i = 0; i < n; i++) {
        printf("String %d: ", i + 1);
        fgets(strings[i], 100, stdin);
        strings[i][strcspn(strings[i], "\n")] = '\0';
    }

    FILE *fp = fopen("output1.txt", "w");
    
    printf("-- Reverse-order output (pointers) --\n");
    fprintf(fp, "-- Reverse-order output (pointers) --\n");
    for (int i = n - 1; i >= 0; i--) {
        reverse_string(strings[i]);
        printf("%d :%s\n", i + 1, strings[i]);
        fprintf(fp, "%d :%s\n", i + 1, strings[i]);
    }

    fclose(fp);

    for (int i = 0; i < n; i++) {
        free(strings[i]);
    }
    free(strings);

    return 0;
}