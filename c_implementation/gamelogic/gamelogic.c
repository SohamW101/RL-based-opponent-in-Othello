// 1 = white, -1 = black, 0 = empty

#include"gamelogic.h"



//deepcopy fn
int** deepcopy_2d_list(int** matrix){

    //allocate memory for rows
    int** copy = (int**)malloc(6 * sizeof(int*));

    if(copy == NULL){
        return NULL;
    }

    for(int i=0; i<6; i++){

        copy[i] = (int*)malloc(6 * sizeof(int));

        if(copy[i] == NULL) {
            // Free allocated memory 
            for(int j = 0; j < i; j++) {
                free(copy[j]);
            }
            free(copy);
            return NULL;  
        }

        //copy elements
        for(int j=0; j<6; j++){

            copy[i][j] = matrix[i][j];

        }
    }

    return copy;
}

//generate board fn
int** generate_board(){

    int** board = (int**)calloc(6,sizeof(int*));        //calloc thus no need to initialise

    if(board == NULL){
        return NULL;
    }

    for(int i=0; i<6; i++){
        board[i] = (int*)calloc(6,sizeof(int));

        if(board[i] == NULL){
            // Free allocated memory 
            for(int j = 0; j < i; j++) {
                free(board[j]);
            }
            free(board);
            return NULL;
        }

    }

        //placing white and black on start positions
        board[3][2] = -1;   //black 
        board[2][3] = -1;   //black
        board[2][2] = 1;    //white
        board[3][3] = 1;    //white

    return board;
    
}

//check for blank fn
bool check_for_blank(int x, int y, int** current_board){ 
    if(current_board[y][x] == 0){
        return true;   //true, space is empty
    }
    else{
        return false;   //space is not empty
    }
}

//toggle fn         white = 1, black = -1                       
int toggle(int whose_turn){
    if(whose_turn == 1){
        return -1;      //switch to black
    }
    else if(whose_turn == -1){
        return 1;       //switch to white
    }
}

//flip pieces fn         
bool flip_pieces(int x, int y, int dx, int dy, int whose_turn, int** current_board){
    int color = whose_turn;
    int color_to_trap = toggle(color);
    
    int new_x = x + dx;
    int new_y = y + dy;
    

    //flip positions 
    int flip_positions[36][2];  //max 36 flips in a 6x6 board
    int index1=0;

    while(new_x >=0 && new_x < 6 && new_y >= 0 && new_y < 6 ){
        int next_cell_color = current_board[new_y][new_x];

        if(next_cell_color == color_to_trap){
           
                flip_positions[index1][0] = new_x;
                flip_positions[index1][1] = new_y;
                index1++;

                new_x += dx;
                new_y += dy;

        }

        else if(next_cell_color == color){

            if(index1 > 0){      //ensure atleast 1 is flipped
                for(int i=0; i<index1; i++){

                    int fx = flip_positions[i][0];
                    int fy = flip_positions[i][1];

                    current_board[fy][fx] = color;     //flip to player's colour
                
                }
                return true;       //true
            } 
            return false;       //false
        }
        else{
            return false;       //false
        }
    }  

    return false;
}


//make move fn
bool make_move(int x, int y, int whose_turn, int** current_board){

    //check if it is in bounds
    if(x < 0 || x>=6 || y < 0 || y >= 6){
        return false;       //false
    }

    //check if space is empty
    if(!check_for_blank(x, y, current_board)){
        return false;       //space is not empty
    }

    int directions[8][2] = {{0,1}, {1,0}, {0,-1}, {-1,0},        //horizontal
                            {1,1}, {-1,1}, {1,-1}, {-1,-1}  };  //diagonal
    int flipped = 0;    //flipped = false

    
    //flipping logic
    for(int i=0; i<8; i++){

    int dx = directions[i][0];
    int dy = directions[i][1];

    if(flip_pieces(x, y, dx, dy, whose_turn, current_board) == 1){
        flipped = 1;    //atleast one direction caused flips
    }
    }

    //final placement
    if(flipped == 1){       //flip occurred
        current_board[y][x] = whose_turn;
        return true;       //true
    }
    else{
        return false;       //false
    }  
}

//valid moves
int** valid_moves(int** board_posn, int whose_turn){

    int** valid_moves_arr = (int**)malloc(36 * sizeof(int*));

    if(valid_moves_arr == NULL){
        return NULL;
    }

    int index2=0;

    for(int y=0; y<6; y++){
        for(int x=0; x<6; x++){

            int** temp_board = deepcopy_2d_list(board_posn);
            
            if(temp_board == NULL){
                for(int j=0; j<index2; j++){
                    free(valid_moves_arr[j]);
                }

                free(valid_moves_arr);
                return NULL;
            }

            if(make_move(x, y, whose_turn, temp_board)){

                valid_moves_arr[index2] = (int*)malloc(2 * sizeof(int));

                if(valid_moves_arr[index2] == NULL) {
                    // Free allocated memory 
                    for(int j = 0; j < index2; j++) {
                        free(valid_moves_arr[j]);
                    }
                    free(valid_moves_arr);
                    
                    for(int j = 0; j < 6; j++) {  
                        free(temp_board[j]);
                    }
                    free(temp_board);
                    
                    return NULL;  
                }

                valid_moves_arr[index2][0] = x;
                valid_moves_arr[index2][1] = y;
                index2++;

            }

            for(int j = 0; j < 6; j++) {  
                free(temp_board[j]);
            }
            free(temp_board);
        }
    }

    valid_moves_arr[index2] = NULL;

    return valid_moves_arr;
}
        

        

//auto pass
bool auto_pass(int whose_turn, int** current_board_for_trial){
    for(int i=0; i<6; i++){
        for(int j=0; j<6; j++){
            if(make_move(i, j, whose_turn, current_board_for_trial) == true){
                return false;
            }
        }
    }
    return true;
}

//victory check
void victory_check(int** current_board){

    int black_pieces=0, white_pieces=0;

    for(int i=0; i<6; i++){
        for(int j=0; j<6; j++){
            if(current_board[i][j] == 1){
                white_pieces ++;
            }
            else if(current_board[i][j] == -1){
                black_pieces ++;
            }
        }
    }

    if(black_pieces < white_pieces){
        printf("White wins!");
        return;
    }
    else if(black_pieces > white_pieces){
        printf("Black wins!");
        return;
    }
    else if(black_pieces == white_pieces){
        printf("Tie!");
        return;
    }
}

//print board
void print_board(int** board){
    for(int y=0; y<6; y++){
        for(int x=0; x<6; x++){
            printf("%d ", board[y][x]);
        }
        printf("\n");
    }

    return;
}