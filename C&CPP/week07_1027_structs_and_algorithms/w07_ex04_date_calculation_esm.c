#include <stdio.h>

int main() {
    int E_target, S_target, M_target;
    if (scanf("%d %d %d", &E_target, &S_target, &M_target) != 3) {
        return 1; 
    }

    int year = 1; 

    while (1) {
        int current_E = (year % 15 == 0) ? 15 : year % 15;
        int current_S = (year % 28 == 0) ? 28 : year % 28;
        int current_M = (year % 19 == 0) ? 19 : year % 19;

        if (current_E == E_target && current_S == S_target && current_M == M_target) {
            break; 
        }
        
        year++;
    }
    
    printf("%d",year);
    return 0;
}