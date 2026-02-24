/*
Swap two integers.
1) Print the input values.
2) Implement swap-by-value and print the result inside the function.
3) Print the values in main (should remain unchanged).
4) Implement swap using pointers (call by reference) and print the result.
5) Print the values in main (should be swapped).
*/
#include <stdio.h>
void swap(int A, int B) {
    int temp = A;
    A = B;
    B = temp;
    printf("After value swap(in funtion): A : %d B : %d\n", A, B);  
}
void swap_by_index(int *A, int *B) {
    int temp = *A;
    *A = *B;
    *B = temp;
    printf("After value swap(in funtion): A : %d B : %d\n", *A, *B);

}
int main() {
    int A=10, B=20;

    printf("Before Swap: A : %d B : %d\n", A, B);
    swap(A, B);
    
    printf("After value swap(in main): A : %d B : %d\n", A, B);

  

    swap_by_index(&A, &B);
    printf("After value swap(in main): A : %d B : %d\n", A, B);
    
    return 0;
}
