#include <stdio.h>
#include <malloc.h>
#include <string.h>

void main() {
    int num;
    char *p = (char*)malloc(sizeof(char) * 100);
    if (!p) {
        perror("malloc");
        return;
    }
    p[0] = '\0'; 

    printf("Enter a number (1-1000): ");
    scanf("%d", &num);
    printf("%d = ", num);
    int len = strlen(p);
    
    while (num >= 1000) {
        strcat(p, "M");
        printf("%d + ", 1000);
        num -= 1000;
    }
    while (num >= 900) {
        strcat(p, "CM");
        printf("%d + ", 900);
        num -= 900;
    }
    while (num >= 500) {
        strcat(p, "D");
        printf("%d + ", 500);
        num -= 500;
    }
    while (num >= 400) {
        strcat(p, "CD");
        printf("%d + ", 400);
        num -= 400;
    }
    while (num >= 100) {
        strcat(p, "C");
        printf("%d + ", 100);
        num -= 100;
    }
    while (num >= 90) {
        strcat(p, "XC");
        printf("%d + ", 90);
        num -= 90;
    }
    while (num >= 50) {
        strcat(p, "L");
        printf("%d + ", 50);
        num -= 50;
    }
    while (num >= 40) {
        strcat(p, "XL");
        printf("%d + ", 40);
        num -= 40;
    }
    while (num >= 10) {
        strcat(p, "X");
        printf("%d + ", 10);
        num -= 10;
    }
    while (num >= 9) {
        strcat(p, "IX");
        printf("%d + ", 9);
        num -= 9;
    }
    while (num >= 5) {
        strcat(p, "V");
        printf("%d + ", 5);
        num -= 5;
    }
    while (num >= 4) {
        strcat(p, "IV");
        printf("%d + ", 4);
        num -= 4;
    }
    while (num >= 1) {
        strcat(p, "I");
        printf("%d + ", 1);
        num -= 1;
    }
    
    if (num == 0) {
        if (strlen(p) > 0) {
            printf("\b\b\b= ");
        } else {
            printf("= ");
        }
        len = (int)strlen(p);
    }

    printf("%s , %d \n", p, len); 

    free(p);
}