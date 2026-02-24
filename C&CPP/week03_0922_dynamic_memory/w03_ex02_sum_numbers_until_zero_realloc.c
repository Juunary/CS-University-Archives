/*
Practice: read integers until the user enters 0, then print the sum.
- 0 can be entered as the first value.
- Use malloc/realloc to grow the array.
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>

void main() {
    int *p;
    int i , hap = 0;
    int cnt = 0;
    int data;

    p = (int*)malloc(sizeof(int) * 1);  // Allocate memory for 1 int

    printf( "Number 1: ");
    scanf("%d", &p[0]);
    if ( p[0] == 0 ) {
        printf("Sum of input numbers ==> 0\n");
        return;
    }
    cnt++;

    for ( i = 2; ;i ++){
        printf( "Number %d: ", i);
        scanf("%d", &data);

        if ( data != 0 ){
            cnt++;
            p = (int*)realloc(p, sizeof(int) * cnt); // Reallocate memory
        }
        else{
            break;
        }
        p[cnt - 1] = data;
        cnt++;
    }

    for (i = 0; i < cnt; i++) {
        hap = hap + p[i];
    }
    printf("Sum of input numbers ==> %d\n", hap);

    free(p);  // Free dynamically allocated memory
    return;
}
