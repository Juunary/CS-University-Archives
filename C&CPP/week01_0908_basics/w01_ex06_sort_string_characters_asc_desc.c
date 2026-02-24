#include <stdio.h>
#include <string.h>
char order[16] = {0};

void input_char(char arr[], int *size) {
    
    printf("Enter a string: ");
    scanf("%s", arr);  
    *size = strlen(arr);

    printf("Enter sort order (ascending or descending): ");
    scanf("%s", order);

}


void func_sort(char arr[], int size, char *order) {
    int order_code = 0; 
    if (strcmp(order, "ascending") == 0) {
        order_code = 1;
    } else if (strcmp(order, "descending") == 0) {
        order_code = 2;
    }
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            if ((order_code == 1 && arr[i] > arr[j]) ||
                (order_code == 2 && arr[i] < arr[j])) {
                char temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

void func_print(char arr[], int size) {
    printf("Sorted result: ");
    for (int i = 0; i < size; i++) {
        printf("%c", arr[i]);
    }
    printf("\n");
}

int main() {
    char arr[101];
    int size;
    

    input_char(arr, &size);  
    func_sort(arr, size, order);
    func_print(arr, size);

    return 0;
}
