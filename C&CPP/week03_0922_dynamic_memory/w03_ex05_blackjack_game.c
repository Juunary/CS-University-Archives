#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

int deck[20];
int deck_index;

void Shuffle();
int calculate_sum(const int* cards, int count);
void Initialization(int** user_cards, int* user_count, int* user_sum, int** dealer_cards, int* dealer_count, int* dealer_sum);
void Status(const int* user_cards, int user_count, int user_sum, const int* dealer_cards, int dealer_count, int dealer_sum);
void PrintCard( int cards);
void Hit(int** cards, int* count, int* sum);
void End(int** dealer_cards, int* dealer_count, int* dealer_sum);
void Result(int user_sum, int dealer_sum);

int main() {
    char command[10];

    // User and dealer status
    int* user_cards = NULL;
    int user_count = 0;
    int user_sum = 0;

    int* dealer_cards = NULL;
    int dealer_count = 0;
    int dealer_sum = 0;

    srand((unsigned int)time(NULL));

    while (1) {
        printf("Enter 'Start' to play or 'Quit' to exit: ");
        scanf("%s", command);

        if (strcmp(command, "Start") == 0) {
            deck_index = 0;

            Shuffle();
            Initialization(&user_cards, &user_count, &user_sum, &dealer_cards, &dealer_count, &dealer_sum);
            Status(user_cards, user_count, user_sum, dealer_cards, dealer_count, dealer_sum);

            // User's turn
            while (user_sum <= 21) {
                printf("Enter 'Hit' to get another card or 'Stay' to end your turn: ");
                scanf("%s", command);

                if (strcmp(command, "Hit") == 0) {
                    Hit(&user_cards, &user_count, &user_sum);
                    Status(user_cards, user_count, user_sum, dealer_cards, dealer_count, dealer_sum);
                    if (user_sum > 21) {
                        Result(user_sum, dealer_sum);
                        break;
                    }
                } else if (strcmp(command, "Stay") == 0) {
                    break;
                } else {
                    printf("Invalid command. Please enter 'Hit' or 'Stay'.\n");
                }
            }

            // Dealer's turn
            if (user_sum <= 21) {
                End(&dealer_cards, &dealer_count, &dealer_sum);
            }
            
            
            Status(user_cards, user_count, user_sum, dealer_cards, dealer_count, dealer_sum);
            Result(user_sum, dealer_sum);

            // Free dynamically allocated memory
            free(user_cards);
            free(dealer_cards);
            user_cards = NULL; // Set to NULL for safety
            dealer_cards = NULL;

        } else if (strcmp(command, "Quit") == 0) {
            printf("Exiting the game. Goodbye!\n");
            break;
        } else {
            printf("Invalid command. Please enter 'Start' or 'Quit'.\n");
        }
    }

    return 0;
}

// 1. Create and shuffle a 20-card deck
void Shuffle() {
    int i, j, temp;
    // Build a 20-card deck: 1~13 (A~K) plus 1~7 (total 20 cards)
    for (i = 0; i < 13; i++) {
        deck[i] = i + 1;
    }
    for (i = 0; i < 7; i++) {
        deck[13 + i] = i + 1;
    }
    for (i = 19; i > 0; i--) {
        j = rand() % (i + 1);
        temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

// 2. Deal two cards each
void Initialization(int** user_cards, int* user_count, int* user_sum, int** dealer_cards, int* dealer_count, int* dealer_sum) {
    *user_cards = (int*)malloc(sizeof(int) * 2);
    *dealer_cards = (int*)malloc(sizeof(int) * 2);
    *user_count = 2;
    *dealer_count = 2;
    
    (*user_cards)[0] = deck[deck_index++];
    (*dealer_cards)[0] = deck[deck_index++];
    (*user_cards)[1] = deck[deck_index++];
    (*dealer_cards)[1] = deck[deck_index++];

    *user_sum = calculate_sum(*user_cards, *user_count);
    *dealer_sum = calculate_sum(*dealer_cards, *dealer_count);
}

// 3. Print current cards
void Status(const int* user_cards, int user_count, int user_sum, const int* dealer_cards, int dealer_count, int dealer_sum) {
    printf("User   Cards (%2d): ", user_sum);
    for (int i = 0; i < user_count; i++) {
        PrintCard(user_cards[i]); // Call function directly (ignore return value)
    }
    printf("\n");

    printf("Dealer Cards (%2d): ", dealer_sum);
    for (int i = 0; i < dealer_count; i++) {
        PrintCard(dealer_cards[i]); // Same: call directly
    }
    printf("\n");
}

void PrintCard(int card) {
    switch (card) {
        case 1:  printf("A ");  break;
        case 11: printf("J ");  break;
        case 12: printf("Q ");  break;
        case 13: printf("K ");  break;
        default: printf("%d ", card); break; // For 2-10, print the number as-is
    }
}

// 4. Give the player one more card
void Hit(int** cards, int* count, int* sum) {
    (*count)++; 
    *cards = (int*)realloc(*cards, sizeof(int) * (*count));
    if (*cards == NULL) {
        printf("Memory reallocation failed!\n");
        exit(1);
    }
    (*cards)[*count - 1] = deck[deck_index++];
    *sum = calculate_sum(*cards, *count);
}

// 5. Dealer draws cards by rule
void End(int** dealer_cards, int* dealer_count, int* dealer_sum) {
    while (*dealer_sum <= 16) {
        printf("Dealer hits. (Sum: %d)\n", *dealer_sum);
        Hit(dealer_cards, dealer_count, dealer_sum);
    }
    printf("Dealer stays. (Sum: %d)\n", *dealer_sum);
}

// 6. Determine win/lose
void Result(int user_sum, int dealer_sum) {
    if (user_sum > 21) {
        printf("User Bust! Dealer Wins!\n");
    } else if (dealer_sum > 21) {
        printf("Dealer Bust! User Wins!\n");
    } else if (user_sum > dealer_sum) {
        printf("User Wins!\n");
    } else if (user_sum < dealer_sum) {
        printf("Dealer Wins!\n");
    } else {
        printf("Draw!\n");
    }
}

// Calculate and return the card sum
int calculate_sum(const int* cards, int count) {
    int sum = 0;
    int ace_count = 0; // Count the number of aces
    for (int i = 0; i < count; i++) {
        if (cards[i] >= 11 && cards[i] <= 13) { // Treat J, Q, K as 10 points
            sum += 10;
        } else if (cards[i] == 1) { // Treat Ace(1) as 11 initially
            ace_count++;
            sum += 1;
        }
        else {
            sum += cards[i];
        }
    }
    return sum;
}