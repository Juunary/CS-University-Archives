#include <stdio.h>
#include <string.h>

struct person {
    char name[50];
    char id[20];
};

union id {
    char ssc[20];   
    char phone[15]; 
};

int main() {
    struct person p1;
    union id p1_id;

    strcpy(p1.name, "Chaeyoung");
    strcpy(p1_id.ssc, "990423-1234567"); 

    printf("--- Struct/Union combined example ---\n");
    printf("Name ==> %s\n", p1.name);
    printf("Phone/SSN ==> %s\n", p1_id.ssc); 


    struct person p2;
    union id p2_id;

    strcpy(p2.name, "Jisoo");
    strcpy(p2_id.phone, "010-1234-5678"); 

    printf("\n--- Struct/Union combined example ---\n");
    printf("Name ==> %s\n", p2.name);
    printf("Phone/SSN ==> %s\n", p2_id.phone); 

    return 0;
}