#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void sort_Ascending(char *pointer_array_B[20], int word_count) {
    for (int i = 0; i < word_count - 1; i++) {
        for (int j = 0; j < word_count - 1 - i; j++) {
            if (strcmp(pointer_array_B[j], pointer_array_B[j + 1]) > 0) {
                char *temp = pointer_array_B[j];
                pointer_array_B[j] = pointer_array_B[j + 1];
                pointer_array_B[j + 1] = temp;
            }
        }
    }
}


void sort_Descending(char *pointer_array_B[20], int word_count) {

    int start = 0;
    int end = word_count - 1;
    while (start < end) {
        char *temp = pointer_array_B[start];
        pointer_array_B[start] = pointer_array_B[end];
        pointer_array_B[end] = temp;

        start++;
        end--;
    }
}

void write_result_to_file(const char *filename, char *pointer_array_B[20], int word_count, const char *sort_type, int max_word_len) {
    FILE *wfp = fopen(filename, "w");
    if (wfp == NULL) {
        return;
    }

    fprintf(wfp, "=======sorting Result========\n");

    for (int i = 0; i < word_count; i++) {
        int word_len = strlen(pointer_array_B[i]);
        int padding = max_word_len - word_len; 

        fprintf(wfp, "%2d. [\t\t\t", i + 1);
        
        for(int j = 0; j < padding; j++) {
            fprintf(wfp, " ");
        }
        
        fprintf(wfp, "%s]\n", pointer_array_B[i]);
    }

    fclose(wfp);
}


int data_slice(char *pointer_array_A[5], char *pointer_array_B[20],int *max_word_len) {
    FILE *rfp = fopen("data.txt", "r");
    if (rfp == NULL) {
        return -1;
    }

    char line_buffer[100];
    int line_index = 0;
    int word_count = 0;
    *max_word_len = 0;

    while (fgets(line_buffer, 100, rfp) != NULL && line_index < 5) {
        size_t len = strlen(line_buffer);
        if (len > 0 && (line_buffer[len - 1] == '\n' || line_buffer[len - 1] == '\r')) {
            line_buffer[len - 1] = '\0';
        }

        pointer_array_A[line_index] = (char *)malloc(strlen(line_buffer) + 1);
        if (pointer_array_A[line_index] == NULL) {
            break;
        }
        strcpy(pointer_array_A[line_index], line_buffer);

        
        char *current = pointer_array_A[line_index];
        while (*current != '\0' && word_count < 20) {
            while (*current != '\0' && (*current == '|' || *current == ',')) { 
                current++;
            }
            while (*current != '\0' && !((*current >= 'A' && *current <= 'Z') || (*current >= 'a' && *current <= 'z') || *current == ' ' || *current == '-') && *current != '|' && *current != ',') {
                current++;
            }

            if (*current == '\0'){
                break;
            } 

            char *word_start = current;
            
            while (*current != '\0' && ((*current >= 'A' && *current <= 'Z') || (*current >= 'a' && *current <= 'z') || *current == '-' || *current == ' ')) {
                current++;
            }
            
            size_t word_len = current - word_start;
            

            if (word_len > 0) {
                if ((int)word_len > *max_word_len) {
                    *max_word_len = (int)word_len;
                }
                pointer_array_B[word_count] = (char *)malloc(word_len + 1);
                if (pointer_array_B[word_count] == NULL) {
                    word_count = -1;
                    break;
                }
                strncpy(pointer_array_B[word_count], word_start, word_len);
                pointer_array_B[word_count][word_len] = '\0';
                
                word_count++;
            }
            
            while (*current != '\0' && (*current == '|' || *current == ',')) {
                 current++;
            }
        }
        
        if (word_count == -1) { 
            word_count = 0;
            break;
        }
        line_index++;
    }

    fclose(rfp);
    return word_count;
}


void cleanup_memory(char *pointer_array_A[5], int line_count, char *pointer_array_B[20], int word_count) {
    for (int i = 0; i < line_count; i++) {
        if (pointer_array_A[i] != NULL) {
            free(pointer_array_A[i]);
            pointer_array_A[i] = NULL;
        }
    }
    
    for (int i = 0; i < word_count; i++) {
        if (pointer_array_B[i] != NULL) {
            free(pointer_array_B[i]);
            pointer_array_B[i] = NULL;
        }
    }
}

int main() {
    char *pointer_array_A[5] = { NULL };
    char *pointer_array_B[20] = { NULL }; 
    int max_word_len = 0; 

    int word_count = data_slice(pointer_array_A, pointer_array_B, &max_word_len);

    if (word_count <= 0) {
        fprintf(stderr, "Failed to read/parse words, or no words were extracted.\n");
        cleanup_memory(pointer_array_A, 5, pointer_array_B, 0); 
        return 1;
    }

    
    sort_Ascending(pointer_array_B, word_count);
    write_result_to_file("Result_Ascending.txt", pointer_array_B, word_count, "Ascending", max_word_len);

    sort_Descending(pointer_array_B, word_count);
    write_result_to_file("Result_Descending.txt", pointer_array_B, word_count, "Descending", max_word_len);
    
    cleanup_memory(pointer_array_A, 5, pointer_array_B, word_count);

    return 0;
}

