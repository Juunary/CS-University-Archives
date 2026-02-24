/*
Sum of even numbers (dynamic allocation).
- Read count N, allocate an int array with malloc, read N integers,
  and print the sum of even values.
- Demonstrates malloc/free usage with pointers.
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>

void main(){
    int *p;
    int i, hap = 0;
    int cnt;

    printf("Enter count: ");
    scanf("%d", &cnt);

    p = (int*)malloc(sizeof(int) * cnt);  // Allocate memory for cnt ints

    for (i = 0; i < cnt; i++) {
        printf("Enter integer #%d: ", i + 1);
        scanf("%d", &p[i]);
    }

    for (i = 0; i < cnt; i++) {
        if (p[i] % 2 == 0) {
            hap = hap + p[i];
        }
    }

    printf("Sum of even numbers ==> %d\n", hap);

    free(p);  // Free dynamically allocated memory

}