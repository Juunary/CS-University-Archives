#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Subject {
    char name[50];
    int credits;
    char grade_str[3];
    double grade_point;
    double subject_score; 
};


double get_grade_point(const char *grade) {
    double base_point = 0.0;
    
    if (!grade || grade[0] == '\0') return 0.0;
    char first_char = grade[0];
    
    switch (first_char) {
        case 'A': base_point = 4.0; break;
        case 'B': base_point = 3.0; break;
        case 'C': base_point = 2.0; break;
        case 'D': base_point = 1.0; break;
        case 'F': base_point = 0.0; break;
        default: return 0.0;
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
    
    struct Subject subject; 
    
    int total_credits = 0;
    double total_score_sum = 0.0;
    double gpa = 0.0;
    
    
    fp = fopen("grade.txt", "r");
    if (fp == NULL) {
        return 1;
    }

    char dummy_line[256];
    if (fgets(dummy_line, sizeof(dummy_line), fp) == NULL) {
        fclose(fp);
        return 1;
    }

    printf("--- Scores by subject ---\n");
    printf("Course | Credits | Grade | Grade Points | Score\n");

    
    while (fscanf(fp, "%s %d %s", subject.name, &subject.credits, subject.grade_str) != EOF) {
        
        subject.grade_point = get_grade_point(subject.grade_str);
        subject.subject_score = subject.credits * subject.grade_point;
        
        total_credits += subject.credits;
        total_score_sum += subject.subject_score;
        
        printf("%s | %d | %s | %.1f | %.1f\n", subject.name, subject.credits, subject.grade_str, subject.grade_point, subject.subject_score);
    }

    fclose(fp);

    if (total_credits > 0) {
        gpa = total_score_sum / total_credits;
        printf("\nTotal GPA: %.2f\n", gpa);
    } 

    return 0;
}