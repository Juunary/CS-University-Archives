
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

int main() {
    struct Node* head = NULL;
    struct Node* tail = NULL;
    int input;

    while (1) {
        printf("Enter integers (0 to finish; if negative, delete the value with the same absolute value): ");
        scanf("%d", &input);

        if (input == 0) {
            break;
        }

        else if (input < 0) {
            int abs_value = -input;
            struct Node* current = head;
            struct Node* prev = NULL;

            while (current != NULL) {
                if (current->data == abs_value) {
                    if (prev == NULL) {
                        head = current->next; 
                    } else {
                        prev->next = current->next; 
                    }
                    free(current);
                    break;
                }
                prev = current;
                current = current->next;
            }
            continue; 
        }

        struct Node* new_node = create_node(input);

        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
    }

    printf("Entered integers:\n");
    struct Node* current = head;
    while (current != NULL) {
        printf("%d\n", current->data);
        struct Node* temp = current;
        current = current->next;
        free(temp);
    }

    return 0;
}