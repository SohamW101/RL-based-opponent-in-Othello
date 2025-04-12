#ifndef BASIC_H
#define BASIC_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "NN.h"

// gamelogic
int **deepcopy_2d_list(int **matrix)
{

    // allocate memory for rows
    int **copy = (int **)malloc(6 * sizeof(int *));

    if (copy == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < 6; i++)
    {

        copy[i] = (int *)malloc(6 * sizeof(int));

        if (copy[i] == NULL)
        {
            // Free allocated memory
            for (int j = 0; j < i; j++)
            {
                free(copy[j]);
            }
            free(copy);
            return NULL;
        }

        // copy elements
        for (int j = 0; j < 6; j++)
        {

            copy[i][j] = matrix[i][j];
        }
    }

    return copy;
}

// generate board fn
int **generate_board()
{

    int **board = (int **)calloc(6, sizeof(int *)); // calloc thus no need to initialise

    if (board == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < 6; i++)
    {
        board[i] = (int *)calloc(6, sizeof(int));

        if (board[i] == NULL)
        {
            // Free allocated memory
            for (int j = 0; j < i; j++)
            {
                free(board[j]);
            }
            free(board);
            return NULL;
        }
    }

    // placing white and black on start positions
    board[3][2] = -1; // black
    board[2][3] = -1; // black
    board[2][2] = 1;  // white
    board[3][3] = 1;  // white

    return board;
}

// check for blank fn
bool check_for_blank(int x, int y, int **current_board)
{
    if (current_board[y][x] == 0)
    {
        return true; // true, space is empty
    }
    else
    {
        return false; // space is not empty
    }
}

// toggle fn         white = 1, black = -1
int toggle(int whose_turn)
{
    if (whose_turn == 1)
    {
        return -1; // switch to black
    }
    else if (whose_turn == -1)
    {
        return 1; // switch to white
    }
}

// flip pieces fn
bool flip_pieces(int x, int y, int dx, int dy, int whose_turn, int **current_board)
{
    int color = whose_turn;
    int color_to_trap = toggle(color);

    int new_x = x + dx;
    int new_y = y + dy;

    // flip positions
    int flip_positions[36][2]; // max 36 flips in a 6x6 board
    int index1 = 0;

    while (new_x >= 0 && new_x < 6 && new_y >= 0 && new_y < 6)
    {
        int next_cell_color = current_board[new_y][new_x];

        if (next_cell_color == color_to_trap)
        {

            flip_positions[index1][0] = new_x;
            flip_positions[index1][1] = new_y;
            index1++;

            new_x += dx;
            new_y += dy;
        }

        else if (next_cell_color == color)
        {

            if (index1 > 0)
            { // ensure atleast 1 is flipped
                for (int i = 0; i < index1; i++)
                {

                    int fx = flip_positions[i][0];
                    int fy = flip_positions[i][1];

                    current_board[fy][fx] = color; // flip to player's colour
                }
                return true; // true
            }
            return false; // false
        }
        else
        {
            return false; // false
        }
    }

    return false;
}

// Add this function after check_for_blank() and before flip_pieces()

bool is_valid_move(int x, int y, int whose_turn, int **current_board) {
    // Check if position is in bounds
    if (x < 0 || x >= 6 || y < 0 || y >= 6) {
        return false;
    }

    // Check if position is empty
    if (!check_for_blank(x, y, current_board)) {
        return false;
    }

    // Check all 8 directions for valid flips
    int directions[8][2] = {
        {0, 1}, {1, 0}, {0, -1}, {-1, 0},  // horizontal and vertical
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1} // diagonal
    };

    for (int i = 0; i < 8; i++) {
        int dx = directions[i][0];
        int dy = directions[i][1];
        
        // Make temporary copy of board to check flips
        int **temp_board = deepcopy_2d_list(current_board);
        if (flip_pieces(x, y, dx, dy, whose_turn, temp_board)) {
            // Free temporary board
            for (int j = 0; j < 6; j++) {
                free(temp_board[j]);
            }
            free(temp_board);
            return true;
        }
        // Free temporary board
        for (int j = 0; j < 6; j++) {
            free(temp_board[j]);
        }
        free(temp_board);
    }

    return false;
}

// make move fn
bool make_move(int x, int y, int whose_turn, int **current_board)
{

    // check if it is in bounds
    if (x < 0 || x >= 6 || y < 0 || y >= 6)
    {
        return false; // false
    }

    // check if space is empty
    if (!check_for_blank(x, y, current_board))
    {
        return false; // space is not empty
    }

    int directions[8][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}, // horizontal
                            {1, 1},
                            {-1, 1},
                            {1, -1},
                            {-1, -1}}; // diagonal
    int flipped = 0;                   // flipped = false

    // flipping logic
    for (int i = 0; i < 8; i++)
    {

        int dx = directions[i][0];
        int dy = directions[i][1];

        if (flip_pieces(x, y, dx, dy, whose_turn, current_board) == 1)
        {
            flipped = 1; // atleast one direction caused flips
        }
    }

    // final placement
    if (flipped == 1)
    { // flip occurred
        current_board[y][x] = whose_turn;
        return true; // true
    }
    else
    {
        return false; // false
    }
}

// valid moves
int **valid_moves(int **board_posn, int whose_turn)
{

    int **valid_moves_arr = (int **)malloc(36 * sizeof(int *));

    if (valid_moves_arr == NULL)
    {
        return NULL;
    }

    int index2 = 0;

    for (int y = 0; y < 6; y++)
    {
        for (int x = 0; x < 6; x++)
        {

            int **temp_board = deepcopy_2d_list(board_posn);

            if (temp_board == NULL)
            {
                for (int j = 0; j < index2; j++)
                {
                    free(valid_moves_arr[j]);
                }

                free(valid_moves_arr);
                return NULL;
            }

            if (make_move(x, y, whose_turn, temp_board))
            {

                valid_moves_arr[index2] = (int *)malloc(2 * sizeof(int));

                if (valid_moves_arr[index2] == NULL)
                {
                    // Free allocated memory
                    for (int j = 0; j < index2; j++)
                    {
                        free(valid_moves_arr[j]);
                    }
                    free(valid_moves_arr);

                    for (int j = 0; j < 6; j++)
                    {
                        free(temp_board[j]);
                    }
                    free(temp_board);

                    return NULL;
                }

                valid_moves_arr[index2][0] = x;
                valid_moves_arr[index2][1] = y;
                index2++;
            }

            for (int j = 0; j < 6; j++)
            {
                free(temp_board[j]);
            }
            free(temp_board);
        }
    }

    valid_moves_arr[index2] = NULL;

    return valid_moves_arr;
}

// auto pass
bool auto_pass(int whose_turn, int **current_board_for_trial)
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (make_move(i, j, whose_turn, current_board_for_trial) == true)
            {
                return false;
            }
        }
    }
    return true;
}

// victory check
void victory_check(int **current_board)
{

    int black_pieces = 0, white_pieces = 0;

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (current_board[i][j] == 1)
            {
                white_pieces++;
            }
            else if (current_board[i][j] == -1)
            {
                black_pieces++;
            }
        }
    }

    if (black_pieces < white_pieces)
    {
        printf("White wins!");
        return;
    }
    else if (black_pieces > white_pieces)
    {
        printf("Black wins!");
        return;
    }
    else if (black_pieces == white_pieces)
    {
        printf("Tie!");
        return;
    }
}

// print board
void print_board(int **board)
{
    printf("\nCurrent board state:\n");
    for (int y = 0; y < 6; y++)
    {
        for (int x = 0; x < 6; x++)
        {
            printf("%2d ", board[y][x]);
        }
        printf("\n");
    }
    printf("\n");
    return;
}

// mobility strategy
double count_valid_moves(int **valid_moves_arr)
{
    int count = 0;

    // arr is empty
    if (valid_moves_arr == NULL)
    {
        return 0;
    }

    // counting array elements
    while (valid_moves_arr[count] != NULL)
    {
        count++;
    }

    return count;
}

double eval(int **board_posn, int whose_turn, int x, int y)
{

    int w1 = 10;
    int w2 = 1;

    // victory check
    int **temp_board4 = deepcopy_2d_list(board_posn);

    if (auto_pass(whose_turn, temp_board4) == true && auto_pass(toggle(whose_turn), board_posn) == true)
    {
        return 0;
    }

    int **temp_board = deepcopy_2d_list(board_posn);
    make_move(x, y, whose_turn, temp_board);

    int **temp_board2 = deepcopy_2d_list(temp_board);
    int **temp_board3 = deepcopy_2d_list(temp_board);

    double count1 = count_valid_moves(valid_moves(temp_board, whose_turn));
    double count2 = count_valid_moves(valid_moves(temp_board2, toggle(whose_turn)));

    int corner_count = 0;

    int coord[4][2] = {{0, 0}, {5, 0}, {0, 5}, {5, 5}};

    for (int i = 0; i < 4; i++)
    {
        if (temp_board3[coord[i][0]][coord[i][1]] == whose_turn)
        {
            corner_count += 1;
        }
    }

    return w1 * (count1 - count2) / (count1 + count2 + 1) + w2 * corner_count;
}

typedef struct
{
    int best_x;
    int best_y;
} move;

move strategy(int **board_posn, int whose_turn)
{
    int **temp_board = deepcopy_2d_list(board_posn);

    int **available_moves = valid_moves(temp_board, whose_turn);

    double max_eval = -1000;     // set to a very low value
    move best_coords = {-1, -1}; // initialising with invalid coords

    if (available_moves != NULL)
    {
        for (int i = 0; available_moves[i] != NULL; i++)
        {

            double current_eval = eval(board_posn, whose_turn, available_moves[i][0], available_moves[i][1]);
            // had to change variable name from eval to current_eval :))
            if (current_eval > max_eval)
            {
                max_eval = current_eval;
                best_coords.best_x = available_moves[i][0];
                best_coords.best_y = available_moves[i][1];
            }
        }

        for (int i = 0; available_moves[i] != NULL; i++)
        {
            free(available_moves[i]);
        }
        free(available_moves);
    }

    return best_coords;
}

// posn strategy

typedef struct
{
    int best_x;
    int best_y;
} move2;

int posn_values[6][6] = {
    {100, -20, 10, 10, -20, 100},
    {-20, -50, -2, -2, -50, -20},
    {10, -2, -1, -1, -2, 10},
    {10, -2, -1, -1, -2, 10},
    {-20, -50, -2, -2, -50, -20},
    {100, -20, 10, 10, -20, 100}};

bool is_endgame(int **board_posn)
{
    int occupied_squares = 0;
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (board_posn[i][j] != 0)
            {
                occupied_squares += 1;
            }
        }
    }

    // corner occupancy
    int all_corners_filled = 0;

    if (
        board_posn[0][0] != 0 && // top left
        board_posn[0][5] != 0 && // top right
        board_posn[5][0] != 0 && // botton left
        board_posn[5][5] != 0    // bottom right
    )
    {
        all_corners_filled = 1; // all corners are filled
    }

    // endgame trigger
    if (occupied_squares > 27 || all_corners_filled == 1)
    {
        return true; // game enters final phase
    }
    else
    {
        return false; // still in middle game
    }
}

// changed the name of fn from eval to chck
int check(int **board_posn, int whose_turn)
{
    if (is_endgame(board_posn) == true)
    { // endgame eval
        int nplayer = 0;
        int nopponent = 0;

        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                if (board_posn[i][j] == whose_turn)
                {
                    nplayer += 1;
                }
                else if (board_posn[i][j] == -1 * whose_turn)
                {
                    nopponent += 1;
                }
            }
        }
        return nplayer - nopponent;
    }
    else
    { // positional eval
        int total_eval = 0;

        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                if (board_posn[i][j] == whose_turn)
                {
                    total_eval += posn_values[i][j];
                }
            }
        }

        return total_eval;
    }
}

void free_2d_array(int **array)
{
    if (array == NULL)
    {
        return;
    }

    for (int i = 0; i < 6; i++)
    {
        if (array[i] != NULL)
        {
            free(array[i]);
        }
    }

    free(array);
}

// this fn is the same as strategy fn in posn_strategy python code
move2 strat(int **board_posn, int whose_turn)
{
    move2 best_coords = {-1, -1}; // initialising with invalid coords
    double best_eval = -10000;

    int success_flag = 1;

    int **temp_board = NULL;

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            temp_board = deepcopy_2d_list(board_posn);

            if (make_move(i, j, whose_turn, temp_board) == true)
            {
                int move_eval = check(temp_board, whose_turn);

                if (move_eval > best_eval)
                {
                    best_eval = move_eval;
                    best_coords.best_x = i;
                    best_coords.best_y = j;
                    success_flag = 0;
                }
            }

            free_2d_array(temp_board);
        }
    }

    if (success_flag == 0)
    {
        return best_coords;
    }
    else
    {
        best_coords.best_x = -1; // returns invalid values
        best_coords.best_y = -1;
        return best_coords;
    }
}













//save_weights
void save_weights(const char *filename, nn* nnpointer)
{
    FILE* fptr = fopen(filename, "w");
    if (!fptr) {
        printf("Error: Could not open file %s for writing\n", filename);
        return;
    }

    for(int i = 0; i < nnpointer->input_size; i++) {
        for (int j = 0; j < nnpointer->hidden_size; j++) {
            fprintf(fptr, "%.6f ", nnpointer->w1[j][i]);
        }
        fprintf(fptr, "\n");
    }

    for (int i = 0; i < nnpointer->hidden_size; i++) {
        fprintf(fptr, "%.6f ", nnpointer->b1[i]);
    }
    fprintf(fptr, "\n");

    for(int i = 0; i < nnpointer->hidden_size; i++) {
        for (int j = 0; j < nnpointer->output_size; j++) {
            fprintf(fptr, "%.6f ", nnpointer->w2[j][i]);
        }
        fprintf(fptr, "\n");
    }

    for (int i = 0; i < nnpointer->output_size; i++) {
        fprintf(fptr, "%.6f ", nnpointer->b2[i]);
    }
    fprintf(fptr, "\n");

    fclose(fptr);
}

nn* load_weights(const char* filename, nn* nnpointer)
{
    nn* temp = nnpointer;
    FILE* fptr = fopen(filename, "r");
    
    if (!fptr) {
        printf("error in load weights\n", filename);
        return NULL;
    }

    for(int row = 0; row < temp->input_size; row++)
    {
        for (int column = 0; column < temp->hidden_size; column++)
        {
            if(fscanf(fptr, "%lf", &temp->w1[column][row]) != 1)
            {
                printf("error w1 row %d column %d\n", row, column);
                fclose(fptr);
                return NULL;
            }
        }
    }

    for (int i = 0; i < temp->hidden_size; i++)
    {
        if(fscanf(fptr, "%lf", &temp->b1[i]) != 1)
        {
            printf("error b1 %d\n", i);
            fclose(fptr);
            return NULL;
        }
    }

    for(int row = 0; row < temp->hidden_size; row++)
    {
        for (int column = 0; column < temp->output_size; column++)
        {
            if(fscanf(fptr, "%lf", &temp->w2[column][row]) != 1)
            {
                printf("error w2 row %d column %d\n", row, column);
                fclose(fptr);
                return NULL;
            }
        }
    }

    for (int i = 0; i < temp->output_size; i++)
    {
        if(fscanf(fptr, "%lf", &temp->b2[i]) != 1)
        {
            printf("error b2 %d\n", i);
            fclose(fptr);
            return NULL;
        }
    }

    fclose(fptr);
    return temp;
}


#endif
