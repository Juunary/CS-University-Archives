#include <stdio.h>
#include <malloc.h>

struct Node {
    int data;
    struct Node* prev;
    struct Node* next;
};

struct Node* create_node(int data){

    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));

    new_node->data = data;
    new_node->prev = NULL;
    new_node->next = NULL;

    return new_node;
}

struct Node* insert_node(struct Node* head, int data) {
    struct Node* new_node = create_node(data);

    if(head==NULL){
        head = new_node;
    }
    else{
        struct Node* temp = head;
        while(temp->next != NULL){
            temp = temp->next;
        }
        new_node->prev = temp;
        temp->next = new_node;
        new_node->prev = temp;
    }
    return head; 
}

struct Node* delete_node(struct Node* head, int target) {
    struct Node* current = head;
    struct Node* temp = NULL;

    // head
    if(current != NULL && current->data == target){
        head = current->next;
        if(head != NULL){
            head->prev = NULL;
        }
        free(current);
        return head;
    }
    // tail
    while(current->next !=NULL && current->data != target){
        current = current->next;
    }
    
    if(current->data == target && current ->next == NULL){
        current->prev->next = NULL;
        free(current);
        return head;
    }
    // else
    if(current->data == target){
        temp = current->prev;
        temp -> next = current->next;
        current->next->prev = temp;
        free(current);
        return head;
    }
    
    printf("Value not found.\n");
    return head;    

}

int main() {
    struct Node* head = NULL;
    int input, choice;

    while (1) {
        printf("Enter integers (0 to finish): ");
        scanf("%d", &input);

        if (input == 0) {
            break;
        }

        head = insert_node(head, input);
    }

    printf("Enter value to delete: ");
    scanf("%d", &choice);
    head = delete_node(head, choice);

    printf("Final list:\n");
    struct Node* current = head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");

    return 0;
}