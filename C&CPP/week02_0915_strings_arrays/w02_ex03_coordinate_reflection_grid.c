#include <stdio.h>

void print_plane(char plane[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf(" %c ", plane[i][j]);
        }
        printf("\n");
    }
}

int main() {
    
    char original_plane[4][4]; 
    char x_reflected_plane[4][4]; 
    char y_reflected_plane[4][4];  
    char origin_reflected_plane[4][4];
    
    int x_coord, y_coord; 
    int x_index, y_index; 
    int i, j; 

    
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            original_plane[i][j] = 'O';
            x_reflected_plane[i][j] = 'O';
            y_reflected_plane[i][j] = 'O';
            origin_reflected_plane[i][j] = 'O';
        }
    }

    printf("Enter coordinates: ");
    scanf("%d %d", &x_coord, &y_coord);

   
    if (x_coord == -2) x_index = 0;
    else if (x_coord == -1) x_index = 1;
    else if (x_coord == 1) x_index = 2;
    else if (x_coord == 2) x_index = 3;
    

    y_coord = -y_coord;
    if (y_coord == -2) y_index = 0;
    else if (y_coord == -1) y_index = 1;
    else if (y_coord == 1) y_index = 2;
    else if (y_coord == 2) y_index = 3;
    else {
        printf("Invalid coordinates.\n");
        return 1;
    }

    original_plane[y_index][x_index] = 'X';


    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (original_plane[i][j] == 'X') {
                x_reflected_plane[4 - 1 - i][j] = 'X';
            }
        }
    }


    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (original_plane[i][j] == 'X') {
                y_reflected_plane[i][4 - 1 - j] = 'X';
            }
        }
    }


    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (original_plane[i][j] == 'X') {
                origin_reflected_plane[4 - 1 - i][4 - 1 - j] = 'X';
            }
        }
    }

    printf("\n Original \n");
    print_plane(original_plane);

    printf("\n X-axis reflection\n");
    print_plane(x_reflected_plane);

    printf("\n Y-axis reflection\n");
    print_plane(y_reflected_plane);

    printf("\n Origin reflection\n");
    print_plane(origin_reflected_plane);

    return 0;
}