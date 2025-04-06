#ifndef MOBILITY_STRATEGY_H
#define MOBILITY_STRATEGY_H

typedef struct
{
    int best_x;
    int best_y;
} move;

double count_valid_moves(int **valid_moves_arr);
double eval(int **board_posn, int whose_turn, int x, int y);
move strategy(int **board_posn, int whose_turn);

#endif