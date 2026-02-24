
#include <stdio.h>
#include <stdlib.h>

bool check_out(int x, int y){
    if(x<1 || x>8 || y<1 || y>8){
        return true;
    }
    return false;
}
bool stone_hit(int king_x, int king_y, int stone_x, int stone_y){
    if(king_x == stone_x && king_y == stone_y){
        return true;
    }
    return false;
}
void move_king(int* king_x, int* king_y, int* stone_x, int* stone_y, char direction){
    switch(direction){
        case 'R':
            if(check_out(*king_x + 1, *king_y)){
                return;
            }
            else if(stone_hit(*king_x + 1, *king_y, *stone_x, *stone_y)){
                if(check_out(*stone_x + 1, *stone_y)){
                    return;
                }
                (*stone_x)++;
            }
            (*king_x)++;
            break;
        case 'L':
            if(check_out(*king_x - 1, *king_y)){
                return;
            }
            (*king_x)--;
            break;
        case 'B':
            if(check_out(*king_x, *king_y - 1)){
                return;
            }
            (*king_y)--;
            break;
        case 'T':
            if(check_out(*king_x, *king_y + 1)){
                return;
            }
            (*king_y)++;
            break;
        case 'RT':
            if(check_out(*king_x + 1, *king_y + 1)){
                return;
            }
            (*king_x)++;
            (*king_y)++;
            break;
        case 'LT':
            if(check_out(*king_x - 1, *king_y + 1)){
                return;
            }
            (*king_x)--;
            (*king_y)++;
            break;
        case 'RB':
            if(check_out(*king_x + 1, *king_y - 1)){
                return;
            }
            (*king_x)++;
            (*king_y)--;
            break;
        default:
            break;
    }
}

int main(){

    // Read the king position, stone position, and number of moves.
    int king_x, stone_x;
    int king_y, stone_y;
    
    int n;

    getchar();
    scanf("%d %d", &king_x, &king_y);
    getchar();
    scanf("%d %d", &stone_x, &stone_y);
    scanf("%d", &n);

    int chess[8][8] = {0,};
    char* move = (char*)malloc(n * sizeof(char));
    for(int i=0; i<n; i++){
        getchar();
        scanf("%c", &move[i]);
        
    }

    free(move);
    return 0;

}