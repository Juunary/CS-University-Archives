#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int book_id;
    char title[100];
    int total_count;
    int current_count;
} Book;

Book booklist[30];
int book_count = 0;

int rented_book_ids[100];
int rented_count = 0;

void initial_booklist();
void manage_booklist(int book_id);
void print_booklist();
void print_inventory();
bool admin_check();
void admin_mode();

void initial_booklist() {
    FILE *fp = fopen("books.txt" , "r");
    char line[200];
    int count_buffer;

    if (fp == NULL) { 
        exit(1); 
    }
    
    if (fgets(line, sizeof(line), fp) == NULL) { 
        fclose(fp); 
    } 

    book_count = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        
        line[strcspn(line, "\n")] = '\0';
        size_t len = strlen(line);
        if (len == 0) continue;

        int i = (int)len - 1;
        int count_end = -1;
        int count_start = -1;

        while (i >= 0 && (line[i] == ' ' || line[i] == '\t')) {
            i--;
        }
        count_end = i; 

        while (i >= 0 && (line[i] >= '0' && line[i] <= '9')) {
            i--;
        }
        count_start = i + 1; 

        if (count_start <= count_end) {
            
            if (count_start > 0 && (line[count_start - 1] == ' ' || line[count_start - 1] == '\t')) {
                
                sscanf(&line[count_start], "%d", &count_buffer);
                
                line[count_start - 1] = '\0';
                
                int title_len = (int)strlen(line);
                int j = title_len - 1;
                while (j >= 0 && (line[j] == ' ' || line[j] == '\t')) {
                    line[j] = '\0';
                    j--;
                }
                
                strncpy(booklist[book_count].title, line, sizeof(booklist[0].title) - 1);
                booklist[book_count].title[sizeof(booklist[0].title) - 1] = '\0';

                booklist[book_count].book_id = book_count;
                booklist[book_count].total_count = count_buffer;
                booklist[book_count].current_count = count_buffer;
                
                book_count++;
                if (book_count >= 30) break; 
            } 
        } 
    }

    fclose(fp);
}

void print_booklist() {
    for (int i = 0; i < book_count; i++) {
        printf(" %2d | %-25s | %10d\n", 
               booklist[i].book_id, 
               booklist[i].title, 
               booklist[i].current_count);
    }
}

void print_inventory() {
    for (int i = 0; i < book_count; i++) {
        printf(" %2d | %-25s | %7d | %8d\n", 
               booklist[i].book_id, 
               booklist[i].title, 
               booklist[i].total_count,
               booklist[i].current_count);
    }
}

void manage_booklist(int book_id) {
    booklist[book_id].current_count--;
    
    if (rented_count < 100) {
        rented_book_ids[rented_count++] = book_id;
    }
}

bool admin_check() {
    FILE *fp = fopen("admin_auth.txt", "r");
    char file_id_key[10], file_pw_key[10];
    char stored_id[50], stored_pw[50];
    char input_id[50], input_pw[50];
    bool success = false;

    if (fp == NULL) {
        return false;
    }

    if (fscanf(fp, "%9s %49s", file_id_key, stored_id) != 2) {
        fclose(fp);
        return false;
    }

    if (fscanf(fp, "%9s %49s", file_pw_key, stored_pw) != 2) {
        fclose(fp);
        return false;
    }
    
    fclose(fp);

    printf("\n--- Admin Login ---\n");
    printf("Enter admin ID: ");
    scanf("%49s", input_id);
    printf("Enter password: ");
    scanf("%49s", input_pw);
    
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 

    if (strcmp(input_id, stored_id) == 0 && strcmp(input_pw, stored_pw) == 0) {
        success = true;
    } else {
        success = false;
    }

    return success;
}

void admin_mode() {
    if (admin_check()) {
        print_inventory();

        char choice[10];
        printf("Continue and switch to service mode? (Y/N): ");
        if (scanf("%9s", choice) == 1) {
            
            int c;
            while ((c = getchar()) != '\n' && c != EOF); 

            if (choice[0] == 'Y' || choice[0] == 'y') {
                printf("\n--- Switching to service mode ---\n");
            } else {
                printf("\n--- Exiting program ---\n");
                exit(0);
            }
        }
    } else {
        printf("\n--- Admin authentication failed. Exiting program. ---\n");
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    
    initial_booklist(); 
    
    if (argc > 1 && strcmp(argv[1], "admin") == 0) {
        admin_mode(); 
    }
    
    int requested_id;
    
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
    
    while (1) {
        print_booklist();

        printf("Enter book ID (exit: -1): ");
        if (scanf("%d", &requested_id) != 1) {
            while ((c = getchar()) != '\n' && c != EOF); 
            continue;
        }
        
        while ((c = getchar()) != '\n' && c != EOF); 
        
        if (requested_id == -1) {
            printf("\n--- Exiting service mode. ---\n");
            break;
        }

        if (requested_id >= 0 && requested_id < book_count) {
            if (booklist[requested_id].current_count > 0) {
                printf("Book ID %d checked out\n", requested_id);
                manage_booklist(requested_id); 
            } else {
                printf("Book ID %d is currently unavailable (stock 0)\n", requested_id);
            }
        } else {
            printf("Invalid book ID.\n");
        }
    }
    
    return 0;
}