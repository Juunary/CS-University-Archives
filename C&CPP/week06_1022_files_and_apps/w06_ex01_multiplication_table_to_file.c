#include <stdio.h>

int main() {
    int table; 
    FILE *file_ptr; 
    char filename[20]; 

    printf("Enter a multiplication table (dan) to print: ");
    scanf("%d", &table);

    snprintf(filename, sizeof(filename), "%d_dan.txt", table);

    file_ptr = fopen(filename, "w");

    if (file_ptr == NULL) {
        printf("file err");
        return 0; 
    }

    for (int i = 1; i < 10; i++) {
        fprintf(file_ptr, "%d X %d = %d\n", table, i, table * i);
    }

    fclose(file_ptr);

    printf("Saved result to %d_dan.txt\n", table);

    return 0;
}