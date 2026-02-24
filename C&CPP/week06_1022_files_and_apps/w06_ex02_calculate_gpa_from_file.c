#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double get_grade_point(const char *grade) {
    double base_point = 0.0;
    
    char first_char = grade[0];
    
    switch (first_char) {
        case 'A':
            base_point = 4.0; 
            break;
        case 'B':
            base_point = 3.0; 
            break;
        case 'C':
            base_point = 2.0;
            break;
        case 'D':
            base_point = 1.0; 
            break;
        case 'F':
            base_point = 0.0;
            break;
        default:
            return 0.0;
    }
    
    if (first_char != 'F') {
        if (strlen(grade) > 1 && grade[1] == '+') {
            base_point += 0.5;
        }
    }

    return base_point;
}


int main() {
    FILE *fp;
    char subject_name[50]; 
    int credits;          
    char grade_str[3];     
    double grade_point;   
    double subject_score;      
    int total_credits = 0;
    double total_score_sum = 0.0;
    double gpa = 0.0;

    fp = fopen("grade.txt", "r");
    if (fp == NULL) {
        perror("Error opening grade.txt");
        printf(" Make sure grade.txt is in the program working directory.\n");
        return 1;
    }

    char dummy_line[256];
    if (fgets(dummy_line, sizeof(dummy_line), fp) == NULL) {
        printf("Error: grade.txt is empty or cannot be read.\n");
        fclose(fp);
        return 1;
    }

    while (fscanf(fp, "%s %d %s", subject_name, &credits, grade_str) != EOF) {
        
        grade_point = get_grade_point(grade_str);
        
        subject_score = credits * grade_point;
        
        total_credits += credits;
        total_score_sum += subject_score;
        
        printf("%s | %d | %s | %1f | %1f\n", 
               subject_name, credits, grade_str, grade_point, subject_score);
    }

    fclose(fp);

    
    if (total_credits > 0) {
        gpa = total_score_sum / total_credits;
        printf("Total GPA: %.2f\n", gpa);
    } 

    return 0;
}