#ifndef POSN_STRATEGY_H
#define POSN_STRATEGY_H

typedef struct{
    int best_x;
    int best_y;
}  move2;

bool is_endgame(int** board_posn);
int check(int** board_posn, int whose_turn);
move2 strat(int** board_posn, int whose_turn);

#endif