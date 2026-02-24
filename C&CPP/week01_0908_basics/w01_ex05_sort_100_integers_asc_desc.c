/*
Sorting (ascending/descending).
- Use an array of 100 integers.
- Implement sort_Ascending() and sort_Descending().
- In main, choose the order and print the array before/after sorting.
*/
#include <stdio.h>

void sort_Ascending(int arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}
void sort_Descending(int arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] < arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}
void print_array(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {

    int choice;

    int arr[100] = {1, 3, 5 , 13 , 7, 9, 11, 15, 17, 19,
            41, 43, 45, 47, 49, 51, 53, 55, 57, 59,
            21, 23, 25, 27, 29, 31, 33, 35, 37, 39,
            61, 63, 65, 67, 69, 71, 73, 75, 77, 79,
            81, 83, 85, 87, 89, 91, 93, 95, 97, 99,
            101, 103, 105, 107, 109, 111, 113, 115, 117, 119,
            1, 3, 5 , 13 , 7, 9, 11, 15, 17, 19,
            41, 43, 45, 47, 49, 51, 53, 55, 57, 59,
            21, 23, 25, 27, 29, 31, 33, 35, 37, 39,
            61, 63, 65, 67, 69, 71, 73, 75, 77, 79};
    printf("Select sort order:\n");
    printf("1. Ascending\n");
    printf("2. Descending\n");
    printf("Choice: ");
    scanf("%d", &choice);

    printf("Array before sorting: ");
    print_array(arr, 100);

    if (choice == 1) {
        sort_Ascending(arr, 100);
        printf("Array after ascending sort: ");
    } else if (choice == 2) {
        sort_Descending(arr, 100);
        printf("Array after descending sort: ");
    } else {
        printf("Invalid choice.\n");
        return 1;
    }

    print_array(arr, 100);
    
    return 0;
}