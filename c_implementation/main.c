#include "q_learning.h"
#include "NN/NN.h"


void play_against_human_and_train(nn* nnpointer, int **board_posn, int whose_turn, int train_as, double epsilon, double learning_rate) {
    if (is_board_full(board_posn)) {
        victory_check(board_posn);
        return;
    }

    int rl_turn = train_as;

    int **valid_moves_arr = valid_moves(board_posn, whose_turn);

    if (whose_turn == rl_turn) {
        // RL's turn
        move_coord* chosen_move = select_move(board_posn, whose_turn, nnpointer, epsilon);

        if (!chosen_move) {
            if (prev_pass_flag == 0) {
                prev_pass_flag = 1;
                play_against_human_and_train(nnpointer, board_posn, toggle(whose_turn), train_as, epsilon, learning_rate);
            } 
            else {
                victory_check(board_posn);               
                return;
            }
        } else {
            prev_pass_flag = 0;
            make_move(chosen_move->x, chosen_move->y, whose_turn, board_posn);
            print_board(board_posn);
            play_against_human_and_train(nnpointer, board_posn, toggle(whose_turn), train_as, epsilon, learning_rate);
        }
    } 
    
    else {
        // Human's turn
        int x, y;
        if (num_valid_moves(valid_moves_arr) == 0) {
            if (prev_pass_flag == 0) {
                prev_pass_flag = 1;
                play_against_human_and_train(nnpointer, board_posn, toggle(whose_turn), train_as, epsilon, learning_rate);
            } 
            else {
                victory_check(board_posn);               
                return;
            }
        }
        else{
            printf("Your turn! Enter your move (x y): ");
            scanf("%d %d", &x, &y);
        }

        if (make_move(x, y, whose_turn, board_posn)) {
            prev_pass_flag = 0;
            print_board(board_posn);
            play_against_human_and_train(nnpointer, board_posn, toggle(whose_turn), train_as, epsilon, learning_rate);
        } 
        else{
            printf("Invalid move! Try again.\n");
            play_against_human_and_train(nnpointer, board_posn, whose_turn, train_as, epsilon, learning_rate);
        }
    }
}


void main(){
    FILE *fptr2 = fopen("weights.txt","r+");
    int input_size = 36;
    int hidden_size = 24;
    int output_size = 36;
    nn *nnpointer = (nn*)malloc(sizeof(nn));
    nnpointer->b1 = (double*)malloc(sizeof(nnpointer->b1));
    nnpointer->b2 = (double*)malloc(sizeof(nnpointer->b2));
    nnpointer->w1 = (double**)malloc(sizeof(nnpointer->w1));
    nnpointer->w2 = (double**)malloc(sizeof(nnpointer->w2));

    if (fgetc(fptr2) == EOF){
        nnpointer = initialise_nn(36,24,36);

        FILE *fptr1;
        fptr1 = fopen("weights.txt","w");

        for(int i = 0;i<hidden_size;i++){
        fprintf(fptr1,"%lf ",nnpointer->b1[i]);
        }
        
        fprintf(fptr1,"\n");

        for(int i = 0;i<hidden_size;i++){
            for(int j = 0;j<input_size;j++){
                fprintf(fptr1,"%lf ",nnpointer->w1[i][j]);
            }
            fprintf(fptr1,"\n");
        }


        for(int i = 0;i<output_size;i++){
            fprintf(fptr1,"%lf ",nnpointer->b2[i]);
        }

        fprintf(fptr1,"\n");

        for(int i = 0;i<output_size;i++){
            for(int j = 0;j<hidden_size;j++){
                fprintf(fptr1,"%lf ",nnpointer->w2[i][j]);
            }
            fprintf(fptr1,"\n");
        }

        fclose(fptr1);
    }

    else{
        for (int i = 0; i < hidden_size; i++) {
            fscanf(fptr2, "%lf", &(nnpointer->b1[i]));
        }
        
        for (int i = 0; i < hidden_size; i++) {
            for(int j = 0;j < input_size;j++){
                fscanf(fptr2, "%lf", &(nnpointer->w1[i][j]));
            }
        }
        
        for (int i = 0; i < output_size; i++) {
            fscanf(fptr2, "%lf", &(nnpointer->b2[i]));
        }
        
        for (int i = 0; i < output_size; i++) {
            for(int j = 0;j < hidden_size;j++){
                fscanf(fptr2, "%lf", &(nnpointer->w2[i][j]));
            }
        }
    }

    fclose(fptr2);

    int **board_posn = generate_board();
    print_board(board_posn); 
    int whose_turn = -1;
    //train_rl(board_posn,-1,nnpointer,0.1);
    prev_pass_flag = 0;
    self_play_and_train(nnpointer,board_posn,whose_turn,0.1,0.1);
    prev_pass_flag = 0;
   // play_against_human_and_train(nnpointer,board_posn,-1,-1,0.1,0.1);
    prev_pass_flag = 0;
   // play_against_human_and_train(nnpointer,board_posn,1,-1,0.1,0.1);
    prev_pass_flag = 0;
}
