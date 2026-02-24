/*
Sieve of Eratosthenes.
- Starting from 2, repeatedly remove multiples of the smallest remaining number.
- Store 2..30 in an array and print prime numbers.
- Write a function that keeps primes and sets non-primes to 0.
*/
#include <stdio.h>
void sieve_of_eratosthenes(int arr[]) {
    for (int i = 2; i * i < 31; i++) {
        for (int j = i * i; j < 31; j += i) {
            if (arr[j] == 0) {
                continue; 
            }
            if (arr[j] % i == 0) {
                arr[j] = 0;
            }
            else {
                continue;
            }

        }
    }
    printf("Primes between 2 and 30: ");
    for (int i = 2; i < 31; i++) {
        if (arr[i] != 0) {
            printf("%d ", arr[i]);
        }
    }
    printf("\n");
}
int main() {
    int arr[31];
    
    for (int i = 2; i <= 30; i++) {
        arr[i] = i;
    }
    sieve_of_eratosthenes(arr);
    return 0;
}
