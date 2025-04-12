#include <Q_learning/q_learning.h>
#include <NN/NN.h>
int prev_pass_flag = 0;
// void play_against_human_and_train(nn* nnpointer,int **board_posn,int whose_turn,int train_as,double epsilon,double learning_rate){
//     if (is_board_full(board_posn)) {
//         return;
//     }


//     int rl_turn = train_as;
//     move_coord* chosen_move = select_move(board_posn,whose_turn,nnpointer,epsilon);
//     move dum_move = strategy(board_posn,whose_turn); 
//     if (whose_turn == rl_turn){
//         chosen_move = select_move(board_posn,whose_turn,nnpointer,epsilon);
//     }
//     else{
//         int x,y;
//         printf("Enter x & y: ");
//         scanf("%d%d",&x,&y);
//         make_move(x,y,whose_turn,board_posn);
//     }

//     if(!chosen_move){
//         if (prev_pass_flag == 0){
//             prev_pass_flag = 1;
//             play_against_posn_strategy_and_train(nnpointer,board_posn,toggle(whose_turn),train_as,epsilon,learning_rate);
//         }
//         else return;
//     }
//     else{
//         prev_pass_flag = 0;
//         make_move(chosen_move->x,chosen_move->y,whose_turn,board_posn);
//         if(whose_turn == rl_turn){
//             train_rl(board_posn,whose_turn,nnpointer,learning_rate);
//         }
//         play_against_posn_strategy_and_train(nnpointer,board_posn,whose_turn,train_as,epsilon,learning_rate);
//     }
// }

void play_against_human_and_train(nn* nnpointer, int **board_posn, int whose_turn, int train_as, double epsilon, double learning_rate) {
    if (is_board_full(board_posn)) {
        return;
    }

    int rl_turn = train_as;

    if (whose_turn == rl_turn) {
        // RL's turn
        move_coord* chosen_move = select_move(board_posn, whose_turn, nnpointer, epsilon);

        if (!chosen_move) {
            if (prev_pass_flag == 0) {
                prev_pass_flag = 1;
                play_against_posn_strategy_and_train(nnpointer, board_posn, toggle(whose_turn), train_as, epsilon, learning_rate);
            } else return;
        } else {
            prev_pass_flag = 0;
            make_move(chosen_move->x, chosen_move->y, whose_turn, board_posn);
            train_rl(board_posn, whose_turn, nnpointer, learning_rate);
            play_against_posn_strategy_and_train(nnpointer, board_posn, toggle(whose_turn), train_as, epsilon, learning_rate);
        }
    } else {
        // Human's turn
        int x, y;
        printf("Your turn! Enter your move (x y): ");
        scanf("%d %d", &x, &y);

        if (is_valid_move(x, y, whose_turn, board_posn)) {
            prev_pass_flag = 0;
            make_move(x, y, whose_turn, board_posn);
            play_against_human_and_train(nnpointer, board_posn, toggle(whose_turn), train_as, epsilon, learning_rate);
        } else {
            printf("Invalid move! Try again.\n");
            play_against_human_and_train(nnpointer, board_posn, whose_turn, train_as, epsilon, learning_rate);
        }
    }
}

void main(){
    int **board_posn = generate_board();
    print_board(board_posn); 
    nn *nnpointer = initialise_nn(36,24,36);
    int whose_turn = -1;
    train_rl(board_posn,-1,nnpointer,0.1);
    prev_pass_flag = 0;
    self_play_and_train(nnpointer,board_posn,whose_turn,0.1,0.1);
    prev_pass_flag = 0;
    play_against_human_and_train(nnpointer,board_posn,-1,-1,0.1,0.1);
    prev_pass_flag = 0;
    play_against_human_and_train(nnpointer,board_posn,1,-1,0.1,0.1);
    prev_pass_flag = 0;
}
