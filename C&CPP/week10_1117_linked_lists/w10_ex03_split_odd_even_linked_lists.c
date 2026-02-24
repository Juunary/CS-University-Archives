

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

        if (input % 2 == 0) { 
            if (even_head == NULL) {
                even_head = new_node;
                even_tail = new_node;
            } else {
                even_tail->next = new_node;
                even_tail = new_node;
            }
        } else { 
            if (odd_head == NULL) {
                odd_head = new_node;
                odd_tail = new_node;
            } else {
                odd_tail->next = new_node;
                odd_tail = new_node;
            }
        }
    }

    printf("Odd list:\n");
    printlist(odd_head);

    printf("Even list:\n");
    printlist(even_head);

    return 0;
}

