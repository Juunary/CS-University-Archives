
#include <stdio.h>

void main(){
    enum months { January=1, February, March, April, May, June, July, August, September, October, November, December};
    enum months m;
    printf("Enter month (1-12): ");
    int month;
    scanf("%d", &month);
    m = month;
    switch(m){
        case January:
            printf("Month %d is January.\n", m);
            break;
        case February:
            printf("Month %d is February.\n", m);
            break;
        case March:
            printf("Month %d is March.\n", m);
            break;
        case April:
            printf("Month %d is April.\n", m);
            break;
        case May:
            printf("Month %d is May.\n", m);
            break;
        case June:
            printf("Month %d is June.\n", m);
            break;
        case July:
            printf("Month %d is July.\n", m);
            break;
        case August:
            printf("Month %d is August.\n", m);
            break;
        case September:
            printf("Month %d is September.\n", m);
            break;
        case October:
            printf("Month %d is October.\n", m);
            break;
        case November:
            printf("Month %d is November.\n", m);
            break;
        case December:
            printf("Month %d is December.\n", m);
            break;
    }
}