#include"gamelogic.h"
#include "mobility_strategy.h"

double count_valid_moves(int** valid_moves_arr){
    int count = 0;

    //arr is empty
    if(valid_moves_arr == NULL){
        return 0;
    }

    //counting array elements
    while(valid_moves_arr[count] != NULL){
        count++;
    }

    return count;
}

double eval(int** board_posn,int whose_turn, int x, int y){

    int w1 = 10;
    int w2 = 1;

    //victory check
    int** temp_board4 = deepcopy_2d_list(board_posn);

    if(auto_pass(whose_turn, temp_board4) == true && auto_pass(toggle(whose_turn), board_posn) == true){
        return 0;
    }
    
    int** temp_board = deepcopy_2d_list(board_posn);
    make_move(x, y, whose_turn, temp_board);

    int** temp_board2 = deepcopy_2d_list(temp_board);
    int** temp_board3 = deepcopy_2d_list(temp_board);

    double count1 = count_valid_moves(valid_moves(temp_board, whose_turn));
    double count2 = count_valid_moves(valid_moves(temp_board2, toggle(whose_turn)));

    int corner_count = 0;

    int coord[4][2] = { {0,0}, {5,0}, {0,5}, {5,5} };

    for(int i=0; i<4; i++){
            if(temp_board3[coord[i][0]][coord[i][1]] == whose_turn){
                corner_count += 1;
            }
    }

    return w1 * (count1 - count2) / (count1 + count2 + 1) + w2 * corner_count;
}

typedef struct{
    int best_x;
    int best_y;
}  move;

move strategy(int** board_posn, int whose_turn){
    int** temp_board = deepcopy_2d_list(board_posn);

    int** available_moves = valid_moves(temp_board, whose_turn);

    double max_eval = -1000;   //set to a very low value
    move best_coords = {-1, -1};    //initialising with invalid coords

    if(available_moves != NULL){
    for(int i=0; available_moves[i]!= NULL; i++){
      
        double current_eval = eval(board_posn, whose_turn, available_moves[i][0], available_moves[i][1]);
        //had to change variable name from eval to current_eval :))
        if(current_eval > max_eval){
            max_eval = current_eval;
            best_coords.best_x = available_moves[i][0];
            best_coords.best_y = available_moves[i][1];
        }   
    }

    for(int i=0; available_moves[i] != NULL; i++){
        free(available_moves[i]);
    }
    free(available_moves);
    }

    return best_coords;
}
