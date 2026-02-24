#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char* my_str_dupe(const char* s)
{
    if (!s) return NULL;
    size_t len = strlen(s);
    char* p = (char*)malloc(len + 1);
    if (!p) return NULL;
    memcpy(p, s, len + 1);
    return p;
}

int cmp_str_no_case(const char* aaa, const char* bbb)
{
    unsigned char ch_a, ch_b;
    while (*aaa && *bbb) {
        ch_a = (unsigned char)tolower((unsigned char)*aaa);
        ch_b = (unsigned char)tolower((unsigned char)*bbb);
        if (ch_a != ch_b) return (ch_a < ch_b) ? -1 : 1;
        ++aaa;
        ++bbb;
    }
    if (*aaa == '\0' && *bbb == '\0') return 0;
    return (*aaa == '\0') ? -1 : 1;
}

static int qsort_compare_func(const void* ptr_a, const void* ptr_b)
{
    const char* const str_a = *(const char* const*)ptr_a;
    const char* const str_b = *(const char* const*)ptr_b;
    return cmp_str_no_case(str_a, str_b);
}

char** create_word_list(const char* big_input_text, int* out_total_words)
{
    if (out_total_words) *out_total_words = 0;
    if (!big_input_text) return NULL;

    char* buffer = my_str_dupe(big_input_text);
    if (!buffer) return NULL;

    size_t capacity = 64;
    size_t current_size = 0;
    char** words_array = (char**)malloc(capacity * sizeof(char*));
    if (!words_array) {
        free(buffer);
        return NULL;
    }

    const char* delims_list = "\t;|,\n\r";
    char* save_ptr_for_strtok = NULL;
    char* token = strtok_r(buffer, delims_list, &save_ptr_for_strtok);
    while (token) {
        while (*token && isspace((unsigned char)*token)) token++;
        size_t the_length = strlen(token);
        while (the_length > 0 && isspace((unsigned char)token[the_length - 1])) token[--the_length] = '\0';

        if (the_length > 0) {
            if (current_size == capacity) {
                capacity <<= 1;
                char** temp_ptr = (char**)realloc(words_array, capacity * sizeof(char*));
                if (!temp_ptr) {
                    for (size_t i = 0; i < current_size; i++) free(words_array[i]);
                    free(words_array);
                    free(buffer);
                    return NULL;
                }
                words_array = temp_ptr;
            }
            char* new_word = my_str_dupe(token);
            if (!new_word) {
                for (size_t i = 0; i < current_size; i++) free(words_array[i]);
                free(words_array);
                free(buffer);
                return NULL;
            }
            words_array[current_size++] = new_word;
        }
        token = strtok_r(NULL, delims_list, &save_ptr_for_strtok);
    }
    free(buffer);

    qsort(words_array, current_size, sizeof(char*), qsort_compare_func);

    if (out_total_words) *out_total_words = (int)current_size;
    return words_array;
}

static char* load_file_to_memory(const char* file_path, size_t* out_size)
{
    if (out_size) *out_size = 0;
    FILE* file_pointer = fopen(file_path, "rb");
    if (!file_pointer) return NULL;
    if (fseek(file_pointer, 0, SEEK_END) != 0) {
        fclose(file_pointer);
        return NULL;
    }
    long file_len = ftell(file_pointer);
    if (file_len < 0) {
        fclose(file_pointer);
        return NULL;
    }
    rewind(file_pointer);

    char* file_data_buffer = (char*)malloc((size_t)file_len + 1);
    if (!file_data_buffer) {
        fclose(file_pointer);
        return NULL;
    }
    size_t read_bytes = fread(file_data_buffer, 1, (size_t)file_len, file_pointer);
    fclose(file_pointer);
    file_data_buffer[read_bytes] = '\0';
    if (out_size) *out_size = read_bytes;
    return file_data_buffer;
}

int main(void)
{
    size_t len_read = 0;
    char* all_text = load_file_to_memory("words.txt", &len_read);
    if (!all_text) {
        perror("Failed to open words.txt");
        return 1;
    }

    int word_count = 0;
    char** dict_arr = create_word_list(all_text, &word_count);
    free(all_text);

    if (!dict_arr) {
        fprintf(stderr, "Failed to create word list\n");
        return 1;
    }

    FILE* output_file = fopen("dictionary.txt", "w");
    if (!output_file) {
        perror("Failed to open dictionary.txt");
        for (int i = 0; i < word_count; i++) free(dict_arr[i]);
        free(dict_arr);
        return 1;
    }
    for (int i = 0; i < word_count; i++) {
        fprintf(output_file, "%s\n", dict_arr[i]);
    }
    fclose(output_file);

    for (int i = 0; i < word_count; i++) free(dict_arr[i]);
    free(dict_arr);

    return 0;
}