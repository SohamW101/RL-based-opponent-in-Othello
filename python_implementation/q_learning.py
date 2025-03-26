import copy
import random

import nn
import mobility_strategy
import posn_strategy
import gamelogic
import store_weights

# prerequisites
def all_posns():    # just gives out all possible coordinates
    coords = []
    for i in range(6):
        for j in range(6):
            coords.append([i, j])
    return coords

def flatten_board(board_posn):  # forward in nn requires flattened (1d array of 36 squares) - so convert accordingly
    flat_board = []
    for row in board_posn:
        for cell in row:
            flat_board.append(cell)
    return flat_board

def valid_move_indices(valid_moves_arr):
    indices = []
    for move in valid_moves_arr:
        x = move[0]
        y = move[1]
        index = y * 6 + x
        indices.append(index)
    return indices


# DQN

# reward function based on mobility player evaluation formula
def reward_fn(board_posn, whose_turn, x, y):
    return mobility_strategy.eval(board_posn, whose_turn, x, y)

def forward_q_values(board_posn, whose_turn, W1, b1, W2, b2 ):
    """weights = store_weights.load_weights()
    if weights is None:
        W1, b1, W2, b2 = nn.initialize_nn(36, 44, 36)  # Initialize if no saved weights
    else:
        W1, b1, W2, b2 = weights  # Load saved weights"""

    valid_moves_arr = gamelogic.valid_moves(board_posn, whose_turn)

    #print("Inside forward_q_values fn")
    #print("valid moves")
    #print(valid_moves_arr)
    flattened_board = flatten_board(board_posn)
    flattened_valid_move_indices = valid_move_indices(valid_moves_arr)

    A1, A2, Z1, Z2 = nn.forward(flattened_board, W1, b1, W2, b2)

    for move in range(36):
        if move not in flattened_valid_move_indices:
            A2[move] = -10000     # set to a very low value

    best_move_index = max(range(36), key=lambda move: A2[move])

    return A1, A2, Z1, Z2, best_move_index

def target_q_values(board_posn, whose_turn, W1, b1, W2, b2):
    valid_moves_arr = gamelogic.valid_moves(board_posn, whose_turn)
    q_targets = []
    gamma = 0.9     # set to 0.9

    #print("inside target q values")
    #print("valid moves")
    #print(valid_moves_arr)
    #gamelogic.print_board(board_posn)

    for i in range(36):
        x = i % 6
        y = i // 6
        if [x, y] in valid_moves_arr:
            reward = reward_fn(board_posn, whose_turn, x, y)

            temp_board = gamelogic.deepcopy_2d_list(board_posn)
            gamelogic.make_move(x, y, whose_turn, temp_board)

            _, next_A2, _, _, _ = forward_q_values(temp_board, gamelogic.toggle(whose_turn), W1, b1, W2, b2)
            q_target_move = reward + gamma * max(next_A2)
        else:
            q_target_move = -10000

        q_targets.append(q_target_move)

    gamelogic.print_board(board_posn)

    return q_targets


def train_rl(board_posn, whose_turn, W1, b1, W2, b2, epochs = 3, learning_rate = 0.1):
    for epoch in range(epochs):
        loss = 0
        #gamelogic.print_board(board_posn)
        input_board = flatten_board(board_posn)
        q_targets = target_q_values(board_posn, whose_turn, W1, b1, W2, b2)

        A1, A2, Z1, Z2, best_move_index = forward_q_values(board_posn, whose_turn, W1, b1, W2, b2)
        W1, b1, W2, b2 = nn.backward(input_board, q_targets, A1, A2, Z1, Z2, W1, b1, W2, b2, learning_rate)

        target_move_index = max(range(36), key=lambda move: q_targets[move])

        loss += (q_targets[target_move_index] - A2[best_move_index]) ** 2

        if epoch % 1 == 0:
            print(f"Epoch {epoch}, Loss = {loss / 36}")
            print()
            print()
    return W1, b1, W2, b2


def predict_qs(board_posn, whose_turn, W1, b1, W2, b2 ):
    _, A2, _, _, best_move_index = forward_q_values(board_posn, whose_turn, W1, b1, W2, b2 )
    return A2, best_move_index


# play
def select_move(board_posn, whose_turn, epsilon=0.1):
    valid_moves = gamelogic.valid_moves(board_posn, whose_turn)

    if not valid_moves:
        return None
    
    # Exploration
    if random.random() < epsilon:
        return random.choice(valid_moves)
    # Exploitation
    A2, _ = predict_qs(board_posn, whose_turn, W1, b1, W2, b2)
    
    max_q_pred = -10000     # initialize to a low value

    best_move = None
    for move in valid_moves:
        x, y = move
        idx = y * 6 + x
        if A2[idx] > max_q_pred:
            max_q_pred = A2[idx]
            best_move = [x, y]

    return best_move


# GAME TRAINING against self and positional strategy player

def self_play_and_train(W1, b1, W2, b2, board_posn, whose_turn, prev_pass_flag = 0):    # recursive function
    chosen_move = select_move(board_posn, whose_turn)    
    if not chosen_move:
        if prev_pass_flag == 0:
            prev_pass_flag = 1
            self_play_and_train(W1, b1, W2, b2, board_posn, gamelogic.toggle(whose_turn), prev_pass_flag)
        else:
            return
    else:
        prev_pass_flag = 0
        gamelogic.make_move(chosen_move[0], chosen_move[1], whose_turn, board_posn)
        train_rl(board_posn, whose_turn,  W1, b1, W2, b2)    # train on updated state and turn
        self_play_and_train( W1, b1, W2, b2, board_posn, gamelogic.toggle(whose_turn), prev_pass_flag)



def play_against_posn_strategy_and_train(W1, b1, W2, b2, board_posn, whose_turn, train_as, prev_pass_flag=0):   # again - recursive
    rl_turn = train_as  # RL agent plays as -1 (black) or 1 (white)

    if whose_turn == rl_turn:
        chosen_move = select_move(board_posn, whose_turn)
    else:
        chosen_move = posn_strategy.strategy(board_posn, whose_turn)

    if not chosen_move:
        if prev_pass_flag == 0:
            prev_pass_flag = 1
            play_against_posn_strategy_and_train(W1, b1, W2, b2, board_posn, gamelogic.toggle(whose_turn), train_as, prev_pass_flag)
        else:
            return  # both passed, game ends
    else:
        prev_pass_flag = 0
        gamelogic.make_move(chosen_move[0], chosen_move[1], whose_turn, board_posn)
        if whose_turn == rl_turn:
            train_rl(board_posn, whose_turn, W1, b1, W2, b2)  # train RL agent on its move
        play_against_posn_strategy_and_train(W1, b1, W2, b2, board_posn, gamelogic.toggle(whose_turn), train_as, prev_pass_flag)



# execute the training
board_posn = gamelogic.generate_board()
W1, b1, W2, b2 = nn.initialize_nn(36, 24, 36)

train_rl(board_posn, -1, W1, b1, W2, b2)

self_play_and_train(W1, b1, W2, b2, board_posn, -1)

play_against_posn_strategy_and_train(W1, b1, W2, b2, board_posn, -1, -1)
play_against_posn_strategy_and_train(W1, b1, W2, b2, board_posn, 1, -1)



"""for game in range(5):
    current_board = gamelogic.deepcopy_2d_list(board_posn)
    self_play_and_train(W1, b1, W2, b2, current_board, -1)

for game in range(5):
    current_board = gamelogic.deepcopy_2d_list(board_posn)
    current_board2 = gamelogic.deepcopy_2d_list(board_posn)
    play_against_posn_strategy_and_train(W1, b1, W2, b2, current_board, -1, -1)
    play_against_posn_strategy_and_train(W1, b1, W2, b2, current_board2, -1, 1)"""


