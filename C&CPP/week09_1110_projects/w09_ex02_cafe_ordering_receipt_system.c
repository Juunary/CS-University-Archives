#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char name[30];
    double price;
} MenuItem;

typedef struct {
    char name[30];
    double u_price;
    int qty;
    double t_price;
} OrderItem;

MenuItem *d_menu = NULL;
int d_cnt = 0;
MenuItem *s_menu = NULL;
int s_cnt = 0;

OrderItem *order_arr = NULL;
int order_cnt = 0;
double g_total = 0.0;

int init_menus(const char *d_file, const char *s_file);
int load_menu_file(const char *fname, MenuItem **m_ptr, int *c_ptr);
void display_menu(const char *title, MenuItem *menu, int count);
void process_drink();
void process_dessert();
void add_order(const char *name, double price);
void print_receipt(const char *fname);

int load_menu_file(const char *fname, MenuItem **m_ptr, int *c_ptr) {
    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        return 0;
    }

    char name[30];
    double price;
    int count = 0;

    long current_pos = ftell(fp);
    while (fscanf(fp, "%s %lf", name, &price) == 2) {
        count++;
    }
    
    fseek(fp, current_pos, SEEK_SET);

    *c_ptr = count;
    
    *m_ptr = (MenuItem *)malloc(count * sizeof(MenuItem));
    if (*m_ptr == NULL) {
        fclose(fp);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        if (fscanf(fp, "%s %lf", (*m_ptr)[i].name, &(*m_ptr)[i].price) != 2) {
            free(*m_ptr);
            *m_ptr = NULL;
            *c_ptr = 0;
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return 1;
}

int init_menus(const char *d_file, const char *s_file) {
    int res = 1;
    
    if (!load_menu_file(d_file, &d_menu, &d_cnt)) res = 0;
    if (!load_menu_file(s_file, &s_menu, &s_cnt)) res = 0;
    
    return res;
}

void display_menu(const char *title, MenuItem *menu, int count) {
    printf("\n%s menus\n", title);
    for (int i = 0; i < count; i++) {
        printf("[%d] %-20s %.2lf\n", i + 1, menu[i].name, menu[i].price);
    }
}

void add_order(const char *name, double price) {
    for (int i = 0; i < order_cnt; i++) {
        if (strcmp(order_arr[i].name, name) == 0) {
            order_arr[i].qty++;
            order_arr[i].t_price += price;
            g_total += price;
            return;
        }
    }

    order_cnt++;
    order_arr = (OrderItem *)realloc(order_arr, order_cnt * sizeof(OrderItem));
    if (order_arr == NULL) {
        exit(1);
    }

    OrderItem *new_item = &order_arr[order_cnt - 1];
    strcpy(new_item->name, name);
    new_item->u_price = price;
    new_item->qty = 1;
    new_item->t_price = price;
    g_total += price;
}

void process_drink() {
    int sel;
    display_menu("drink", d_menu, d_cnt);
    printf("Select a drink to order\n:: ");
    
    if (scanf("%d", &sel) != 1) {
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (sel >= 1 && sel <= d_cnt) {
        MenuItem selected = d_menu[sel - 1];
        add_order(selected.name, selected.price);
    } else {
        printf("Invalid selection.\n");
    }
}

void process_dessert() {
    int sel;
    display_menu("dessert", s_menu, s_cnt);
    printf("Select a dessert to order\n:: ");
    
    if (scanf("%d", &sel) != 1) {
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (sel >= 1 && sel <= s_cnt) {
        MenuItem selected = s_menu[sel - 1];
        add_order(selected.name, selected.price);
    } else {
        printf("Invalid selection.\n");
    }
}

void print_receipt(const char *fname) {
    FILE *fp = fopen(fname, "w");
    if (fp == NULL) {
        return;
    }

    void output_receipt(FILE *stream) {
        fprintf(stream, "===========================\n");
        fprintf(stream, "          Receipt          \n");
        fprintf(stream, "===========================\n");
        
        if (order_cnt == 0) {
            fprintf(stream, "No items were ordered.\n");
        } else {
            for (int i = 0; i < order_cnt; i++) {
                OrderItem item = order_arr[i];
                fprintf(stream, "%-20s %d %8.2lf\n", 
                        item.name, item.qty, item.t_price);
            }
        }
        
        fprintf(stream, "===========================\n");
        fprintf(stream, "Total :: %20.2lf\n", g_total);
    }

    output_receipt(fp);
    fclose(fp);

    printf("\nReceipt saved to %s.\n", fname);
    printf("==================================================\n");
    output_receipt(stdout); 
    printf("==================================================\n");
}

int main() {
    
    if (!init_menus("drink.txt", "dessert.txt")) {
        printf("Unable to load the menu file.\n");
        return 1;
    }

    int sel;
    
    while (1) {
        printf("\n1. Order a drink\n2. Order a dessert\n0. Exit\n:: ");
        if (scanf("%d", &sel) != 1) {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        if (sel == 0) {
            break;
        } else if (sel == 1) {
            process_drink();
        } else if (sel == 2) {
            process_dessert();
        } else {
            printf("Invalid selection. Please try again.\n");
        }
    }

    print_receipt("Receipt.txt");

    if (d_menu != NULL) free(d_menu);
    if (s_menu != NULL) free(s_menu);
    if (order_arr != NULL) free(order_arr);

    return 0;
}