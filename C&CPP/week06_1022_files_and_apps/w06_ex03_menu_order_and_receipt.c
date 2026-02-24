#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[30]; 
    int price;   
} MenuItem;

typedef struct {
    char name[30];
    int price;
} SelectedMenu;

MenuItem *menu_list = NULL;
int menu_count = 0;

int load_menu(const char *filename);
void display_menu(void);
void process_order(void);
void generate_receipt(const char *filename, SelectedMenu **selected_items, int selected_count, int total_amount);

int load_menu(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening menu.txt");
        return 0;
    }

    char dummy_line[256];
    if (fgets(dummy_line, sizeof(dummy_line), fp) == NULL) {
        printf("Error: menu.txt is empty or cannot be read.\n");
        fclose(fp);
        return 0;
    }

    int count = 0;
    char name[30];
    char price_str[10]; 
    long current_pos = ftell(fp); 
    
    while (fscanf(fp, "%s %s", name, price_str) == 2) {
        count++;
    }
    
    fseek(fp, current_pos, SEEK_SET);

    menu_count = count;
    
    menu_list = (MenuItem *)malloc(menu_count * sizeof(MenuItem));
    

    for (int i = 0; i < menu_count; i++) {
        fscanf(fp, "%s %s", menu_list[i].name, price_str);
        menu_list[i].price = atoi(price_str);
    }

    fclose(fp);
    return 1;
}


void display_menu(void) {

    for (int i = 0; i < menu_count; i++) {
        printf("[ %d ] %20s : %6d\n", i + 1, menu_list[i].name, menu_list[i].price);
    }

}


void process_order(void) {
    SelectedMenu **selected_items = NULL; 
    int selected_count = 0;              
    int total_amount = 0;          
    int menu_number;

    while (1) {
        display_menu();
        printf("Choose a menu item (press 0 to finish): ");
        if (scanf("%d", &menu_number) != 1) {
            printf("number err\n");
            while (getchar() != '\n');
            continue;
        }
        
        if (menu_number == 0) {
            break;
        }

        if (menu_number >= 1 && menu_number <= menu_count) {
            selected_items = (SelectedMenu **)realloc(selected_items, (selected_count + 1) * sizeof(SelectedMenu *));
            if (selected_items == NULL) {
                perror("realloc error");
                break;
            }

            SelectedMenu *new_item = (SelectedMenu *)malloc(sizeof(SelectedMenu));
            if (new_item == NULL) {
                perror("alloc error");
                break;
            }

            MenuItem current_menu = menu_list[menu_number - 1];
            strcpy(new_item->name, current_menu.name);
            new_item->price = current_menu.price;

            selected_items[selected_count] = new_item;
            total_amount += new_item->price;
            selected_count++;
            
            printf("%s, order received.\n", new_item->name);

        } else {
            printf("number err\n");
        }
    }
    

    generate_receipt("receipt.txt", selected_items, selected_count, total_amount);


    for (int i = 0; i < selected_count; i++) {
        free(selected_items[i]);
    }
    free(selected_items); 
}


void generate_receipt(const char *filename, SelectedMenu **selected_items, int selected_count, int total_amount) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error opening receipt.txt for writing");
        return;
    }

    if (selected_count == 0) {
        fprintf(fp, "no selected menu\n");
    } else {
        for (int i = 0; i < selected_count; i++) {
            SelectedMenu *item = selected_items[i];
            fprintf(fp, "%s : %d\n", item->name, item->price);
        }
    }

    fprintf(fp, "Payment amount: %d KRW\n", total_amount);

    fclose(fp);
}



int main() {

    // Load menu file
    if (!load_menu("menu.txt")) {
        return 1;
    }

    process_order();

    if (menu_list != NULL) {
        free(menu_list);
    }

    return 0;
}