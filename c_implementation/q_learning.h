#ifndef q_learning
#define q_learning
#include <stdio.h>
#include<stdlib.h>
#include<math.h>
#include "NN.h"
#include "basic.h"

int is_board_full(int **board_posn) // it was printing extra boards after the game ended, this is to check if its happening and to stop it
{
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if (board_posn[i][j] == 0)
            {
                return 0;
            }
        }
    }
    return 1;
}

int **all_posns() // generates all possible positions on the board
{
    int **board = malloc(sizeof(int *) * 6 * 6);
    int index = 0;
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            board[index] = malloc(sizeof(int) * 2);
            if (board[index] == NULL)
            {
                printf("error in allocating memory for board[index]");
                exit(1);
            }
            board[index][0] = i;
            board[index][1] = j;
            index++;
        }
    }

    return board;
}

int *flatten_board(int **board_posn) // 2d board to 1d in order to pass it through the NN
{
    int *flat_board = malloc(sizeof(int) * 6 * 6);
    int index = 0;
    for (int row = 0; row < 6; row++)
    {
        for (int cell = 0; cell < 6; cell++)
        {
            flat_board[index] = board_posn[row][cell];
            index++;
        }
    }
    return flat_board;
}

int *valid_move_indices(int **valid_moves_arr, int num_moves) // used to convert each valid move to their indices
{
    int *indices = malloc(sizeof(int) * num_moves);
    for (int i = 0; i < num_moves; i++)
    {
        int x = valid_moves_arr[i][0];
        int y = valid_moves_arr[i][1];
        indices[i] = y * 6 + x;
    }
    return indices;
}

int num_valid_moves(int **valid_moves_arr) // counts all valid moves, finds NULL
{
    int count = 0;
    if (valid_moves_arr == NULL)
    {
        return 0;
    }

    while (valid_moves_arr[count] != NULL)
    {
        count++;
    }

    return count;
}

double reward_fn(int **board_posn, int whose_turn, int x, int y) // basic reward function used to define where it should take the next move
{
    return eval(board_posn, whose_turn, x, y);
}

typedef struct
{
    double *a1, *a2, *z1, *z2;
    int best_move_index;
} q_values;

q_values *forward_q_values(int **board_posn, int whose_turn, nn *nnpointer)
{
    q_values *qpointer = malloc(sizeof(q_values));

    int **valid_moves_arr = valid_moves(board_posn, whose_turn);
    int num_moves = num_valid_moves(valid_moves_arr);

    int *flattened_board = flatten_board(board_posn);
    int *flattened_valid_move_indices = valid_move_indices(valid_moves_arr, num_moves);

    double *input = malloc(36 * sizeof(double));
    for (int i = 0; i < 36; i++)
    {
        input[i] = (double)flattened_board[i];
    }

    double *a1, *a2, *z1, *z2;
    forward(nnpointer, input, &a1, &a2, &z1, &z2);

    for (int move = 0; move < 36; move++)
    {
        int is_valid = 0;
        for (int i = 0; i < num_moves; i++)
        {
            if (flattened_valid_move_indices[i] == move)
            {
                is_valid = 1;
                break;
            }
        }
        if (!is_valid)
        {
            a2[move] = -10000.0;
        }
    }

    int best_move_index = 0;
    for (int i = 1; i < 36; i++)
    {
        if (a2[i] > a2[best_move_index])
        {
            best_move_index = i;
        }
    }

    qpointer->a1 = a1;
    qpointer->a2 = a2;
    qpointer->z1 = z1;
    qpointer->z2 = z2;
    qpointer->best_move_index = best_move_index;

    free(flattened_board);
    free(flattened_valid_move_indices);
    free(input);

    if (valid_moves_arr != NULL)
    {
        for (int i = 0; i < num_moves; i++)
        {
            free(valid_moves_arr[i]);
        }
    }

    return qpointer;
}

typedef struct
{
    double *qts;
    int size;
} q_targets;

q_targets *target_q_values(int **board_posn, int whose_turn, nn *nnpointer)
{
    q_targets *qtpointer = malloc(sizeof(q_targets));
    if (qtpointer == NULL)
    {
        printf("issue in target_q_values");
        exit(1);
    }

    qtpointer->qts = malloc(36 * sizeof(double));
    if (qtpointer->qts == NULL)
    {
        printf("issue in target_q_values");
        exit(1);
    }

    qtpointer->size = 36;

    int **valid_moves_arr = valid_moves(board_posn, whose_turn);
    int num_moves = num_valid_moves(valid_moves_arr);
    const double gamma = 0.9;

    for (int i = 0; i < 36; i++)
    {
        int x = i % 6;
        int y = i / 6;

        int is_valid = 0;
        for (int j = 0; j < num_moves; j++)
        {
            if (valid_moves_arr[j][0] == x && valid_moves_arr[j][1] == y)
            {
                is_valid = 1;
                break;
            }
        }

        if (is_valid)
        {
            double reward = reward_fn(board_posn, whose_turn, x, y);

            int **temp_board = deepcopy_2d_list(board_posn);
            make_move(x, y, whose_turn, temp_board);

            q_values *next_q = forward_q_values(temp_board, toggle(whose_turn), nnpointer);

            qtpointer->qts[i] = reward + gamma * next_q->a2[next_q->best_move_index];

            for (int j = 0; j < 6; j++)
            {
                free(temp_board[j]);
            }

            free(temp_board);
            free(next_q->a1);
            free(next_q->a2);
            free(next_q->z1);
            free(next_q->z2);
            free(next_q);
        }

        else
        {
            qtpointer->qts[i] = -10000.0;
        }
    }

    if (valid_moves_arr != NULL)
    {
        for (int i = 0; i < num_moves; i++)
        {
            free(valid_moves_arr[i]);
        }
        free(valid_moves_arr);
    }

    return qtpointer;
}

void train_rl(int **board_posn, int whose_turn, nn *nnpointer, double learning_rate)
{
    for (int epoch = 0; epoch < 100; epoch++)
    {
        double loss = 0.0;

        double *input_board = malloc(36 * sizeof(double));
        if (input_board == NULL)
        {
            printf("error in train_rl function");
            exit(1);
        }

        int *flat_board = flatten_board(board_posn);
        for (int i = 0; i < 36; i++)
        {
            input_board[i] = (double)flat_board[i];
        }

        q_targets *qt = target_q_values(board_posn, whose_turn, nnpointer);
        q_values *qv = forward_q_values(board_posn, whose_turn, nnpointer);

        int target_move_index = 0;
        for (int i = 1; i < 36; i++)
        {
            if (qt->qts[i] > qt->qts[target_move_index])
                target_move_index = i;
        }

        backward(nnpointer, input_board, qt->qts, qv->a1, qv->a2, qv->z1, qv->z2, learning_rate);

        loss = pow(qt->qts[target_move_index] - qv->a2[qv->best_move_index], 2);

        printf("epoch %d, loss = %f", epoch + 1, loss / 36.0);
        printf("\n\n");

        save_weights("weights.txt",nnpointer);

        free(input_board);
        free(flat_board);
        free(qt->qts);
        free(qt);
        free(qv->a1);
        free(qv->a2);
        free(qv->z1);
        free(qv->z2);
        free(qv);
    }
}

typedef struct
{
    double *qvs;
    int best_move_index;
} predict_result;

predict_result *predict_qs(int **board_posn, int whose_turn, nn *nnpointer)
{
    predict_result *pred = malloc(sizeof(predict_result));

    if (pred == NULL)
    {
        printf("error in predict_qs function");
        exit(1);
    }

    q_values *qv = forward_q_values(board_posn, whose_turn, nnpointer);
    if (qv == NULL)
    {
        printf("error in predict_qs function");
    }

    pred->qvs = malloc(36 * sizeof(double));
    if (pred->qvs == NULL)
    {
        printf("error in predict_qs function");
        free(pred);
        free(qv->a1);
        free(qv->a2);
        free(qv->z1);
        free(qv->z2);
        free(qv);
        exit(1);
    }

    for (int i = 0; i < 36; i++)
    {
        pred->qvs[i] = qv->a2[i];
    }

    pred->best_move_index = qv->best_move_index;

    free(qv->a1);
    free(qv->a2);
    free(qv->z1);
    free(qv->z2);
    free(qv);

    return pred;
}

typedef struct
{
    int x, y;
} move_coord;

move_coord *select_move(int **board_posn, int whose_turn, nn *nnpointer, double epsilon)
{
    int **valid_moves_arr = valid_moves(board_posn, whose_turn);
    if (valid_moves_arr == NULL)
    {
        printf("error in select move function");
        return NULL;
    }

    int num_moves = num_valid_moves(valid_moves_arr);
    move_coord *selected_move = malloc(sizeof(move_coord));
    if (selected_move == NULL)
    {
        printf("error in select move function");
        return NULL;
    }

    double random = (double)rand() / RAND_MAX;
    if (random < epsilon)
    {
        int random_index = rand() % num_moves;
        selected_move->x = valid_moves_arr[random_index][0];
        selected_move->y = valid_moves_arr[random_index][1];
    }

    else
    {
        predict_result *pred = predict_qs(board_posn, whose_turn, nnpointer);
        if (pred == NULL)
        {
            free(selected_move);
            return NULL;
        }

        double max_q_pred = -10000.0;
        selected_move->x = -1;
        selected_move->y = -1;

        for (int i = 0; i < num_moves; i++)
        {
            int x = valid_moves_arr[i][0];
            int y = valid_moves_arr[i][1];

            int idx = y * 6 + x;

            if (pred->qvs[idx] > max_q_pred)
            {
                max_q_pred = pred->qvs[idx];
                selected_move->x = x;
                selected_move->y = y;
            }
        }

        free(pred->qvs);
        free(pred);
    }

    for (int i = 0; i < num_moves; i++)
    {
        free(valid_moves_arr[i]);
    }
    free(valid_moves_arr);

    return selected_move;
}

int prev_pass_flag = 0;

void self_play_and_train(nn *nnpointer, int **board_posn, int whose_turn, double epsilon, double learning_rate)
{
    if (is_board_full(board_posn))
    {
        return;
    }

    move_coord *chosen_move = select_move(board_posn, whose_turn, nnpointer, epsilon);

    if (!chosen_move)
    {
        if (prev_pass_flag == 0)
        {
            prev_pass_flag = 1;
            self_play_and_train(nnpointer, board_posn, toggle(whose_turn), epsilon, learning_rate);
        }
        else
        {
            return;
        }
    }
    else
    {
        prev_pass_flag = 0;
        make_move(chosen_move->x, chosen_move->y, whose_turn, board_posn);
        print_board(board_posn);
        train_rl(board_posn, whose_turn, nnpointer, learning_rate);
        self_play_and_train(nnpointer, board_posn, toggle(whose_turn), epsilon, learning_rate);
    }
}

void play_against_posn_strategy_and_train(nn *nnpointer, int **board_posn, int whose_turn, int train_as, double epsilon, double learning_rate)
{
    if (is_board_full(board_posn))
    {
        return;
    }

    int rl_turn = train_as;
    move_coord *chosen_move = select_move(board_posn, whose_turn, nnpointer, epsilon);
    move2 dum_move = strat(board_posn, whose_turn);
    if (whose_turn == rl_turn)
    {
        chosen_move = select_move(board_posn, whose_turn, nnpointer, epsilon);
    }
    else
    {
        chosen_move->x = dum_move.best_x;
        chosen_move->y = dum_move.best_y;
    }

    if (!chosen_move)
    {
        if (prev_pass_flag == 0)
        {
            prev_pass_flag = 1;
            play_against_posn_strategy_and_train(nnpointer, board_posn, toggle(whose_turn), train_as, epsilon, learning_rate);
        }
        else
            return;
    }
    else
    {
        prev_pass_flag = 0;
        make_move(chosen_move->x, chosen_move->y, whose_turn, board_posn);
        if (whose_turn == rl_turn)
        {
            train_rl(board_posn, whose_turn, nnpointer, learning_rate);
        }
        play_against_posn_strategy_and_train(nnpointer, board_posn, whose_turn, train_as, epsilon, learning_rate);
    }
}


#endif
