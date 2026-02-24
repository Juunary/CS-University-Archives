#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char name[50];
    float price;
    struct Node* next;
    struct Node* down;
};

struct Node* create_node(char* name, float price) {
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    if (new_node == NULL) {
        exit(1);
    }
    strcpy(new_node->name, name);
    new_node->price = price;
    new_node->next = NULL;
    new_node->down = NULL;
    return new_node;
}

struct Node* insert_node(struct Node* head, char* name, float price) {
    struct Node* new_node = create_node(name, price);

    if (head == NULL) {
        head = new_node;
    }
    else {
        struct Node* temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = new_node;
    }
    return head;
}

struct Node* copy_list(struct Node* source) {
    if (source == NULL) return NULL;

    struct Node* new_head = NULL;
    struct Node* tail = NULL;
    struct Node* current = source;

    while (current != NULL) {
        struct Node* new_node = create_node(current->name, current->price);
        if (new_head == NULL) {
            new_head = new_node;
            tail = new_node;
        }
        else {
            tail->next = new_node;
            tail = new_node;
        }
        current = current->next;
    }
    return new_head;
}

void printlist(struct Node* head) {
    struct Node* current = head;
    while (current != NULL) {
        printf("%s %.2f", current->name, current->price);
        printf("=>");
        current = current->next;
    }
    printf("\n");
}

void swap_data(struct Node* a, struct Node* b) {
    float temp_price = a->price;
    a->price = b->price;
    b->price = temp_price;

    char temp_name[50];
    strcpy(temp_name, a->name);
    strcpy(a->name, b->name);
    strcpy(b->name, temp_name);
}

void sort_by_name(struct Node* head) {
    if (head == NULL) return;
    struct Node* current;
    int swapped;

    do {
        swapped = 0;
        current = head;

        while (current->next != NULL) {
            if (strcmp(current->name, current->next->name) > 0) {
                swap_data(current, current->next);
                swapped = 1;
            }
            current = current->next;
        }
    } while (swapped);
}

void sort_by_price(struct Node* head) {
    if (head == NULL) return;
    struct Node* current;
    int swapped;

    do {
        swapped = 0;
        current = head;

        while (current->next != NULL) {
            if (current->price > current->next->price) {
                swap_data(current, current->next);
                swapped = 1;
            }
            current = current->next;
        }
    } while (swapped);
}

int main() {
    struct Node* name_head = NULL;
    struct Node* price_head = NULL;

    FILE* fp = fopen("drink.txt", "r");
    if (fp == NULL) {
        return 1;
    }

    char name_buffer[50];
    float price_buffer;

    while (fscanf(fp, "%s %f", name_buffer, &price_buffer) != EOF) {
        name_head = insert_node(name_head, name_buffer, price_buffer);
    }
    fclose(fp);

    price_head = copy_list(name_head);

    sort_by_name(name_head);
    sort_by_price(price_head);

    name_head->down = price_head;

    printlist(name_head);
    printf("\\");
    printf("\n");
    printlist(name_head->down);

    struct Node* curr_name = name_head;
    while (curr_name != NULL) {
        struct Node* temp = curr_name;
        curr_name = curr_name->next;
        free(temp);
    }

    struct Node* curr_price = price_head;
    while (curr_price != NULL) {
        struct Node* temp = curr_price;
        curr_price = curr_price->next;
        free(temp);
    }

    return 0;
}