

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Student {
    char name[10];
    int age;
};
int main() {
    int n, i;
    struct Student *students;

    printf("Number of students to input: ");
    scanf("%d", &n);
    

    students = (struct Student *)malloc(n * sizeof(struct Student));
    if (students == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    for (i = 0; i < n; i++) {
        printf("Enter name and age: ");
        scanf("%s %d", students[i].name, &students[i].age);
    }

    printf("\n-- Student roster --\n");
    for (i = 0; i < n; i++) {
        printf("Name: %s, Age: %d\n", students[i].name, students[i].age);
    }

    free(students);
    return 0;
}