import copy
import nn
import mobility_strategy
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

# reward function based on mobility player evaluation formula
def reward_fn(board_posn, whose_turn, x, y):
    return mobility_strategy.eval(board_posn, whose_turn, x, y)

def forward_q_values(board_posn, whose_turn):
    weights = store_weights.load_weights()
    if weights is None:
        W1, b1, W2, b2 = nn.initialize_nn(36, 44, 36)  # Initialize if no saved weights
    else:
        W1, b1, W2, b2 = weights  # Load saved weights

    valid_moves_arr = gamelogic.valid_moves(board_posn, whose_turn)
    flattened_board = flatten_board(board_posn)
    flattened_valid_move_indices = valid_move_indices(valid_moves_arr)

    A1, A2, Z1, Z2 = nn.forward(flattened_board, W1, b1, W2, b2)

    for move in range(36):
        if move not in flattened_valid_move_indices:
            A2[move] = -10000     # set to a very low value

    return A1, A2, Z1, Z2

def target_q_values(board_posn, whose_turn):
    valid_moves_arr = gamelogic.valid_moves(board_posn, whose_turn)
    q_targets = []
    gamma = 0.9     # set to 0.9

    for move in all_posns():
        if move in valid_moves_arr:
            reward = reward_fn(board_posn, whose_turn, move[0], move[1])

            temp_board = copy.deepcopy(board_posn)
            gamelogic.make_move(move[0], move[1], whose_turn, temp_board)

            _, next_A2, _, _ = forward_q_values(temp_board, whose_turn)
            q_target_move = reward + gamma * max(next_A2)  # temp_board has been updated in earlier line
            q_targets.append(q_target_move)
        else:
            q_targets.append(-10000)

    return q_targets

def predict_qs(board_posn, whose_turn):
    _, A2, _, _ = forward_q_values(board_posn, whose_turn)
    return A2

def train_rl(board_posn, whose_turn, learning_rate=0.1, epochs=10000):
    flattened_board = flatten_board(board_posn)

    weights = store_weights.load_weights()
    if weights is None:
        W1, b1, W2, b2 = nn.initialize_nn(36, 44, 36)  # Initialize if no saved weights
    else:
        W1, b1, W2, b2 = weights

    q_targets = target_q_values(board_posn, whose_turn)

    W1, b1, W2, b2 = nn.train_nn([flattened_board], [q_targets], W1, b1, W2, b2, epochs, learning_rate)     # Without [], flattened_board (which is a list of 36 values) would be interpreted as 36 separate training samples, rather than one board state.

    store_weights.save_weights(W1, b1, W2, b2)
