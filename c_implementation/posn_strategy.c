#include "gamelogic.h"


int posn_values[6][6] = {
    {100,  -20,  10,  10,  -20,  100},
    {-20,  -50,  -2,  -2,  -50,  -20},
    {10,    -2,  -1,  -1,   -2,   10},
    {10,    -2,  -1,  -1,   -2,   10},
    {-20,  -50,  -2,  -2,  -50,  -20},
    {100,  -20,  10,  10,  -20,  100}
};

bool is_endgame(int** board_posn){
    int occupied_squares = 0;
    for(int i=0; i<6; i++){
        for(int j=0; j<6; j++){
            if(board_posn[i][j] != 0){
                occupied_squares += 1;
            }
        }
    }

    //corner occupancy
    int all_corners_filled = 0;

    if(
        board_posn[0][0] != 0 &&    //top left
        board_posn[0][5] != 0 &&    //top right
        board_posn[5][0] != 0 &&    //botton left
        board_posn[5][5] != 0       //bottom right
    ){
        all_corners_filled = 1; //all corners are filled
    }

    //endgame trigger
    if(occupied_squares > 27 || all_corners_filled == 1){
        return true;    //game enters final phase
    }
    else{
        return false;   //still in middle game
    }

}

//changed the name of fn from eval to chck
int check(int** board_posn, int whose_turn){
    if(is_endgame(board_posn) == true){ //endgame eval
        int nplayer = 0;
        int nopponent = 0;

        for(int i=0; i<6; i++){
            for(int j=0; j<6; j++){
                if(board_posn[i][j] == whose_turn){
                    nplayer += 1;
                }
                else if(board_posn[i][j] == -1*whose_turn){
                    nopponent += 1;
                }
            }
        }
        return nplayer - nopponent;
    }
    else{       //positional eval
        int total_eval = 0;

        for(int i=0; i<6; i++){
            for(int j=0; j<6; j++){
                if(board_posn[i][j] == whose_turn){
                    total_eval += posn_values[i][j];
                }
            }
        }

        return total_eval;
    }

}

typedef struct{
    int best_x;
    int best_y;
}  move2;

//this fn is the same as strategy fn in posn_strategy python code
move2 strat(int** board_posn, int whose_turn){
    move2 best_coords = {-1, -1};    //initialising with invalid coords
    double best_eval = -10000;

    int success_flag = 1;

    int** temp_board = NULL;

    for(int i=0; i<6; i++){
        for(int j=0; j<6; j++){
            temp_board = deepcopy_2d_list(board_posn);

            if(make_move(i, j, whose_turn, temp_board) == true){
                int move_eval = check(temp_board, whose_turn);

                if(move_eval > best_eval){
                    best_eval = move_eval;
                    best_coords.best_x = i;
                    best_coords.best_y = j;
                    success_flag = 0;
                }
            }

            free_2d_array(temp_board);
        }
    }
 

    if(success_flag == 0){
        return best_coords;
    }
    else{
        best_coords.best_x = -1;  //returns invalid values
        best_coords.best_y = -1;
        return best_coords;
    }
}