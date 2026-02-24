/*
2D array reduction (4x8).
1) Read 8 positive integers (non-zero) into the first row.
2) Sum adjacent pairs and store results in the next row.
3) Repeat for each row and print the final aggregated number.
*/
#include <stdio.h>

int main() {
    int matrix[4][8] = {0}; 
    int i, j;

    printf("Enter 8 num:\n");
    for (i = 0; i < 8; i++) {
        do {
            printf("num %d: ", i + 1);
            scanf("%d", &matrix[0][i]);
        } while (matrix[0][i] <= 0);
    }

    
    int gap = 1;  
    int step = 2; 

    for (i = 1; i < 4; i++) {
        for (j = 0; j < 8; j += step) {
            
            int val1 = *(*(matrix + i - 1) + j);
            int val2 = *(*(matrix + i - 1) + j + gap);

            matrix[i][j] = val1 + val2;
        }
        
        gap *= 2;  
        step *= 2; 
    }

    

    printf("final number: %d\n", *(*(matrix + 3) + 0));

    return 0;
}