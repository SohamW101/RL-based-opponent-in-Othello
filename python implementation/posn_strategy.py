# stategy is to maximize eval till middle game and simply maximize own pieces in the endgame

import gamelogic
import copy

# board_posn is a 2-d list: +1 if white piece, -1 if black, else 0

posn_values = [
[100, -20,  10,  10, -20, 100],
[-20, -50,  -2,  -2, -50, -20],
[ 10,  -2,  -1,  -1,  -2,  10],
[ 10,  -2,  -1,  -1,  -2,  10],
[-20, -50,  -2,  -2, -50, -20],
[100, -20,  10,  10, -20, 100]
]

def is_endgame(board_posn):
    occupied_squares = 0
    for i in range(6):
        for j in range(6):
            if board_posn[i][j] != 0:
                occupied_squares += 1

    all_corners_filled = (
        board_posn[0][0] != 0 and
        board_posn[0][5] != 0 and
        board_posn[5][0] != 0 and
        board_posn[5][5] != 0
    )
    
    if occupied_squares > 27 or all_corners_filled:
        return True
    return False


def eval(board_posn, whose_turn, endgame):      #whose_turn will have value +1 for white and -1 for black
    if endgame:  # Endgame evaluation
        nplayer = 0
        nopponent = 0
        for i in range(6):
            for j in range(6):
                if board_posn[i][j] == whose_turn:
                    nplayer += 1
                elif board_posn[i][j] == -whose_turn:
                    nopponent += 1
        return nplayer - nopponent
    
    else:  # Positional evaluation
        total_eval = 0
        for i in range(6):
            for j in range(6):
                if board_posn[i][j] == whose_turn:
                    total_eval += posn_values[i][j]
        return total_eval
    

def strategy(board_posn, whose_turn):
    best_x = -1
    best_y = -1
    best_eval = -10000  # Start with a very low score

    for i in range(6):
        for j in range(6):
            temp_board = copy.deepcopy(board_posn)  # Copy board to test move

            if gamelogic.make_move(i, j, whose_turn, temp_board):  
                move_eval = eval(temp_board, whose_turn)  
                if move_eval > best_eval:  
                    best_eval = move_eval
                    best_x = i
                    best_y = j

    return best_x, best_y 