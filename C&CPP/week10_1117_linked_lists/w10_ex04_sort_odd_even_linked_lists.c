
#include <stdio.h>
#include <malloc.h>

struct Node {
    int data;
    struct Node* next;
};

struct Node* create_node(int data){

    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));

    new_node->data = data;
    new_node->next = NULL;


    return new_node;

}

void printlist(struct Node* head) {
    struct Node* current = head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

void swap(struct Node* a, struct Node* b) {
    int temp = a->data;
    a->data = b->data;
    b->data = temp;
}

void sort_list(struct Node* head) {
    if (head == NULL) return;

    struct Node* current = head;
    int swapped;

    do {
        swapped = 0;
        current = head;

        while (current->next != NULL) {
            if (current->data > current->next->data) {
                swap(current, current->next);
                swapped = 1;
            }
            current = current->next;
        }
    } while (swapped);
}

int main() {
    struct Node* even_head = NULL;
    struct Node* even_tail = NULL;
    struct Node* odd_head = NULL;
    struct Node* odd_tail = NULL;
    int input;

    while (1) {
        printf("Enter integers (0 to finish): ");
        scanf("%d", &input);

        if (input == 0) {
            break;
        }

        struct Node* new_node = create_node(input);

        if (input % 2 == 0) { // even
            if (even_head == NULL) {
                even_head = new_node;
                even_tail = new_node;
            } else {
                even_tail->next = new_node;
                even_tail = new_node;
            }
        } else { // odd
            if (odd_head == NULL) {
                odd_head = new_node;
                odd_tail = new_node;
            } else {
                odd_tail->next = new_node;
                odd_tail = new_node;
            }
        }
    }

    printf("Odd list (before sort):\n");
    printlist(odd_head);

    sort_list(odd_head);
    printf("Odd list (after sort):\n");
    printlist(odd_head);

    printf("Even list (before sort):\n");
    printlist(even_head);

    sort_list(even_head);
    printf("Even list (after sort):\n");
    printlist(even_head);

    return 0;
}

