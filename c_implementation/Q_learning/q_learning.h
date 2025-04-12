#ifndef q_learning
#define q_learning

#include "basic.h"

typedef struct
{
    double *a1, *a2, *z1, *z2;
    int best_move_index;
} q_values;

typedef struct
{
    double *qts;
    int size;
} q_targets;

typedef struct
{
    double *qvs;
    int best_move_index;
} predict_result;

typedef struct
{
    int x, y;
} move_coord;

int is_board_full(int **board_posn);
int **all_posns();
int *flatten_board(int **board_posn);
int *valid_move_indices(int** valid_moves_arr, int num_moves);
int num_valid_moves(int **valid_moves_arr);
double reward_fn(int **board_posn, int whose_turn, int x, int y);
q_values *forward_q_values(int **board_posn, int whose_turn, nn* nnpointer);
q_targets *target_q_values(int **board_posn, int whose_turn, nn *nnpointer);
void train_rl(int **board_posn, int whose_turn, nn *nnpointer, double learning_rate);
predict_result *predict_qs(int **board_posn, int whose_turn, nn *nnpointer);
move_coord *select_move(int **board_posn, int whose_turn, nn *nnpointer, double epsilon);
void self_play_and_train(nn *nnpointer, int **board_posn, int whose_turn, double epsilon, double learning_rate);
void play_against_posn_strategy_and_train(nn* nnpointer,int **board_posn,int whose_turn,int train_as,double epsilon,double learning_rate);


#endif
