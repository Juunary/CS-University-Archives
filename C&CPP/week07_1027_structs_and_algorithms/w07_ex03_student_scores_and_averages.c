#include <stdio.h>
#include <stdlib.h>

struct student {
    char name[10];
    int kor;
    int eng;
    float avg;
};

struct averages {
    float kor;
    float eng;
    float all;
};

int main(void) {
    int n;
    printf("Enter number of students: ");
    scanf("%d", &n);

    struct student *students = malloc(n * sizeof *students);

    for (int i = 0; i < n; ++i) {
        printf("\nStudent %d name: ", i + 1);
        scanf("%9s", students[i].name);
        printf("Korean score: ");
        scanf("%d", &students[i].kor);
        printf("English score: ");
        scanf("%d", &students[i].eng);
        students[i].avg = (students[i].kor + students[i].eng) / 2.0f;
    }

    float sum_k = 0.0f, sum_e = 0.0f, sum_avg = 0.0f;
    for (int i = 0; i < n; ++i) {
        sum_k += students[i].kor;
        sum_e += students[i].eng;
        sum_avg += students[i].avg;
    }

    struct averages avg = { sum_k / n, sum_e / n, sum_avg / n };

    FILE *fp = fopen("average_strudent.txt", "w");

    fprintf(fp, "Student\tAverage\n");
    for (int i = 0; i < n; ++i) {
        fprintf(fp, "%s\t%.2f\n", students[i].name, students[i].avg);
    }
    fprintf(fp, "\nSubject averages: Korean %.2f\tEnglish %.2f\n", avg.kor, avg.eng);
    fprintf(fp, "Overall average: %.2f\n", avg.all);

    fclose(fp);
    free(students);

    return 0;
}