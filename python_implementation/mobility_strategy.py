import gamelogic
import copy

# stategy is to maximize eval till middle game and simply maximize own pieces in the endgame

# board_posn is a 2-d list: +1 if white piece, -1 if black, else 0
# whose_turn will have value +1 for white and -1 for black


def eval(board_posn, whose_turn, x, y):
    w1 = 10
    w2 = 1

    # victory case
    temp_board4 = gamelogic.deepcopy_2d_list(board_posn)
    if gamelogic.auto_pass(whose_turn, temp_board4) and gamelogic.auto_pass(gamelogic.toggle(whose_turn), board_posn):
        return 0

    temp_board = gamelogic.deepcopy_2d_list(board_posn)
    gamelogic.make_move(x, y, whose_turn, temp_board)   # this will update temp_board to that move
    temp_board2 = gamelogic.deepcopy_2d_list(temp_board)
    temp_board3 = gamelogic.deepcopy_2d_list(temp_board)
    count1 = len(gamelogic.valid_moves(temp_board, whose_turn))
    count2 = len(gamelogic.valid_moves(temp_board2, gamelogic.toggle(whose_turn)))
    
    corner_count = 0
    for coord in [[0, 0], [0, 5], [5, 0], [5, 5]]:
        if temp_board3[coord[0]][coord[1]] == whose_turn:
            corner_count += 1

    return w1 * (count1 - count2) / (count1 + count2 + 1) + w2 * corner_count

def strategy(board_posn, whose_turn):
    temp_board = gamelogic.deepcopy_2d_list(board_posn)
    available_moves = gamelogic.valid_moves(temp_board, whose_turn)

    max_eval = -1000    # set to a very low value 
    for move in available_moves:
        eval = eval(board_posn, whose_turn, move[0], move[1])
        if eval > max_eval:
            max_eval = eval
            best_x = move[0]
            best_y = move[1]

    return [best_x, best_y]
    