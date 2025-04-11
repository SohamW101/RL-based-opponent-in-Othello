#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include "mobility/mobility_strategy.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int **deepcopy_2d_list(int **matrix);
int **generate_board();
bool check_for_blank(int x, int y, int **current_board);
int toggle(int whose_turn);
bool flip_pieces(int x, int y, int dx, int dy, int whose_turn, int **current_board);
bool make_move(int x, int y, int whose_turn, int **current_board);
int **valid_moves(int **board_posn, int whose_turn);
bool auto_pass(int whose_turn, int **current_board_for_trial);
void victory_check(int **current_board);
void print_board(int **board);

#endif