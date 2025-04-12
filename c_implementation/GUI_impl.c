#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <Q_learning/q_learning.h>
#include <NN/NN.h>
int prev_pass_flag = 0;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

#define CELL_SIZE 100
#define RADIUS 40

void draw_circle(SDL_Renderer* renderer, int x, int y, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

move_coord handle_mouse_input(int **board_posn, int whose_turn) {
    SDL_Event e;
    while (SDL_WaitEvent(&e)) {
        if (e.type == SDL_QUIT) {
            exit(0);
        }
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int x = e.button.x / CELL_SIZE;
            int y = e.button.y / CELL_SIZE;
            move_coord coord = {x, y};
            return coord;
        }
    }

}

void render_board(int **board) {
    SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255); // Green background
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Lines
    for (int i = 0; i <= 6; i++) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, 6 * CELL_SIZE);
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, 6 * CELL_SIZE, i * CELL_SIZE);
    }

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            if (board[i][j] != 0) {
                SDL_Color color = (board[i][j] == 1) ? (SDL_Color){0, 0, 0, 255} : (SDL_Color){255, 255, 255, 255};
                draw_circle(renderer, j * CELL_SIZE + CELL_SIZE / 2, i * CELL_SIZE + CELL_SIZE / 2, RADIUS, color);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void play_against_human_and_train(nn* nnpointer, int **board_posn, int whose_turn, int train_as, double epsilon, double learning_rate) {
    if (is_board_full(board_posn)) {
        victory_check(board_posn);
        return;
    }

    int rl_turn = train_as;

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

            render_board(board_posn);
            SDL_Delay(300);

            train_rl(board_posn, whose_turn, nnpointer, learning_rate);
            play_against_human_and_train(nnpointer, board_posn, toggle(whose_turn), train_as, epsilon, learning_rate);
        }
    } else {
        // Human's turn
        move_coord move = handle_mouse_input(board_posn,whose_turn);
        int x = move.x;
        int y = move.y;
        int **valid_moves_arr = valid_moves(board_posn, whose_turn);
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

        if (is_valid_move(x, y, whose_turn, board_posn)) {
            prev_pass_flag = 0;
            make_move(x, y, whose_turn, board_posn);

            render_board(board_posn);
            SDL_Delay(300);

            play_against_human_and_train(nnpointer, board_posn, toggle(whose_turn), train_as, epsilon, learning_rate);
        } 
        else if (!(is_valid_move(x, y, whose_turn, board_posn))){
            printf("Invalid move! Try again.\n");
            play_against_human_and_train(nnpointer, board_posn, whose_turn, train_as, epsilon, learning_rate);
        }
    }
}


void main(){
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("Othello", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 6 * CELL_SIZE, 6 * CELL_SIZE + 100, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    font = TTF_OpenFont("arial.ttf", 28); // Make sure arial.ttf is in your project folder

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

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
