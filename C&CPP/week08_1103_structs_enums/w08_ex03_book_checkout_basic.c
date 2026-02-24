#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
void print_current_booklist();
void main_loop();

void initial_booklist() {
    FILE *fp = fopen("books.txt" , "r");
    char line[100];
    int count_buffer;

    if (fp == NULL) { exit(1); }
    if (fgets(line, sizeof(line), fp) == NULL) { fclose(fp); exit(1); } // Skip header row

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


void print_current_booklist() {

    for (int i = 0; i < book_count; i++) {
        printf(" %2d | %-25s | %10d\n", 
               booklist[i].book_id, 
               booklist[i].title, 
               booklist[i].current_count);
    }
}

void manage_booklist(int book_id) {
    booklist[book_id].current_count--;
    
    if (rented_count < 100) {
        rented_book_ids[rented_count++] = book_id;
    }
}


int main() {

    
    initial_booklist(); 
    int requested_id;
    char buffer[100]; 

    while (1) {
        print_current_booklist();

        printf("Enter book ID (exit: -1): ");
        scanf("%d", &requested_id);
        if (requested_id == -1) {
            break;
        }
        if (booklist[requested_id].current_count > 0) {
            printf("Checked out\n");
            manage_booklist(requested_id); 
        } else {
            printf("Unavailable\n");
        }
    }
    
    return 0;
}