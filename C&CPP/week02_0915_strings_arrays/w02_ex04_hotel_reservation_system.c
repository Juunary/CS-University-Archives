/*
Hotel reservation system (Part 1).
- Read requested number of guests and room type, then decide whether reservation is possible.
Rooms:
  - VIP: max 4 guests, 3 rooms (301, 302, 303)
  - Standard: max 2 guests, 5 rooms (201..205)
- Handle 5 reservation requests and print results.
- If guests=5 with VIP type -> needs 2 VIP rooms.
Functions:
  - reservation_hotel(): reservation logic using vip_check() / normal_check()
  - vip_check(): VIP reservation handling
  - normal_check(): standard reservation handling
*/
#include <stdio.h>

int vip_room_numbers[3];
int vip_room_reserved[3];

int standard_room_numbers[5];
int standard_room_reserved[5];


void initialize_rooms();
void reservation_hotel(int num_people, int room_type_choice);
int vip_check(int num_people);
int normal_check(int num_people);


int main() {
    int num_people;
    int room_type_choice;
    int i;

    initialize_rooms();

    for (i = 0; i < 5; i++) {
        printf("\n--- Reservation %d ---\n", i + 1);
        printf("Number of guests: ");
        scanf("%d", &num_people);
        printf("Room type (1: VIP, 0: Standard): ");
        scanf("%d", &room_type_choice);
        
        reservation_hotel(num_people, room_type_choice);
    }
    return 0;
}

void reservation_hotel(int num_people, int room_type_choice) {
    int success = 0;


    if (room_type_choice == 1) {
        success = vip_check(num_people);
    } else {
        success = normal_check(num_people);
    }

    if (success == 0) {
        printf("Sorry, reservation failed.\n");
    }
}

int vip_check(int num_people) {
    int required_rooms;
    int available_count = 0;
    int i;

    required_rooms = num_people / 4;
    if (num_people % 4 > 0) {
        required_rooms = required_rooms + 1;
    }
    

    for (i = 0; i < 3; i++) {
        if (vip_room_reserved[i] == 0) {
            available_count++;
        }
    }
    
    if (available_count >= required_rooms) {
        printf("Reserved %d VIP room(s) successfully!\n", required_rooms);
        printf("Assigned room(s): ");
        
        for (i = 0; i < 3; i++) {
            if (vip_room_reserved[i] == 0) {
                vip_room_reserved[i] = 1; 
                printf("Room %d ", vip_room_numbers[i]); 
                
                required_rooms--;
                if (required_rooms == 0) {
                    break;
                }
            }
        }
        printf("\n");
        return 1;
    } 
    
    return 0;
}

int normal_check(int num_people) {
    int required_rooms;
    int available_count = 0;
    int i;

    required_rooms = num_people / 2;
    if (num_people % 2 > 0) {
        required_rooms = required_rooms + 1;
    }
    
    for (i = 0; i < 5; i++) {
        if (standard_room_reserved[i] == 0) {
            available_count++;
        }
    }
    
    if (available_count >= required_rooms) {
        printf("Reserved %d standard room(s) successfully!\n", required_rooms);
        printf("Assigned room(s): ");
        
        for (i = 0; i < 5; i++) {
            if (standard_room_reserved[i] == 0) {
                standard_room_reserved[i] = 1;
                printf("Room %d ", standard_room_numbers[i]);
                
                required_rooms--;
                if (required_rooms == 0) {
                    break;
                }
            }
        }
        printf("\n");
        return 1;
    } 
    
    return 0;
}

void initialize_rooms() {
    int i;
    
    for (i = 0; i < 3; i++) {
        vip_room_numbers[i] = 301 + i;
        vip_room_reserved[i] = 0;
    }
    
    for (i = 0; i < 5; i++) {
        standard_room_numbers[i] = 201 + i;
        standard_room_reserved[i] = 0;
    }
}