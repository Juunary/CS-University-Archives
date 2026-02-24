/*
Array swap with pointers.
- Read two integers, store them in an array, and swap the values.
1) Print the input values in main.
2) Swap the two elements via an array parameter.
3) Print the array values in main.
4) Read new values and print them.
5) Swap via element addresses (pointer parameters).
6) Print the final array values in main.
*/
#include <stdio.h>
void swap(int *arr) {
    int temp = arr[0];
    arr[0] = arr[1];
    arr[1] = temp;
    printf("Swap by index: %d %d\n", arr[0], arr[1]);
}
void swap_by_index(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
    printf("Swap by Pointer: %d %d\n", *a, *b);
}
int main() {
    int arr[2];

    int A = 10, B = 20;
    arr[0] = A;
    arr[1] = B;
    printf("Before Swap: %d %d\n", arr[0], arr[1]);
    swap(arr);

    A = 30, B = 40;
    arr[0] = A;
    arr[1] = B;
    printf("Before Swap : %d %d\n", arr[0], arr[1]);
    swap_by_index(&arr[0], &arr[1]);
    return 0;
}