#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void reverse_string(char *str) {
    int len = strlen(str);
    int end = len - 1; 

    if (len > 0 && (str[end] == '\n' || str[end] == '\r')) {
        str[end] = '\0'; 
        len--;
        end--;
    }

    
    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

int main() {
    FILE *rfp;
    FILE *wfp;
    char line[100]; 
    rfp = fopen("in.txt", "r");

    wfp = fopen("output2.txt", "w");


    while (fgets(line, 100, rfp) != NULL) {
        
        reverse_string(line);

       
        printf("%s \n", line);
        
        fprintf(wfp, "%s\n", line); 
    }

    
    fclose(rfp);
    fclose(wfp);

    return 0;
}
