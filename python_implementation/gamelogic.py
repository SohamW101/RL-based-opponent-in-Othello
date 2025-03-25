import copy

# deepcopy doesn't seem to work properly - hence written my own fn
def deepcopy_2d_list(matrix):
    copy = []
    for row in matrix:
        new_row = []
        for item in row:
            new_row.append(item)
        copy.append(new_row)
    return copy



# +1 for white, -1 for black, and 0 for blank
def generate_board():
    board = [[0 for _ in range(6)] for _ in range(6)]
    board[3][2] = -1
    board[2][3] = -1
    board[2][2] = 1
    board[3][3] = 1
    return board



# Game Handling

def check_for_blank(x, y, current_board):
    return current_board[y][x] == 0


def toggle(whose_turn):  # whose_turn is an int: 1 or -1
    return -1 if whose_turn == 1 else 1


def flip_pieces(x, y, dx, dy, whose_turn, current_board):
    color = whose_turn
    color_to_trap = toggle(color)
    new_x, new_y = x + dx, y + dy
    flip_positions = []

    # First step: check if we're still in bounds before accessing board
    while 0 <= new_x < 6 and 0 <= new_y < 6:
        
        next_cell_color = current_board[new_y][new_x]

        if next_cell_color == color_to_trap:
            flip_positions.append((new_x, new_y))
            new_x += dx
            new_y += dy
        elif next_cell_color == color:
            if flip_positions:  # Ensure at least one piece is flipped
                for fx, fy in flip_positions:
                    current_board[fy][fx] = color
                return True
            return False
        else:  # If empty or out of sequence, stop
            return False
    #print(current_board)
    return False  # If it runs out of bounds, flipping is not possible


def make_move(x, y, whose_turn, current_board):
    if not (0 <= x < 6 and 0 <= y < 6):  
        return False

    if not check_for_blank(x, y, current_board): 
        return False  

    directions = [(0, 1), (0, -1), (1, 0), (-1, 0), (1, 1), (-1, 1), (1, -1), (-1, -1)]
    flipped = False

    for dx, dy in directions:
        if flip_pieces(x, y, dx, dy, whose_turn, current_board):
            flipped = True

    if flipped:
        current_board[y][x] = whose_turn  # Place piece only if valid move
        return True

    return False


def valid_moves(board_posn, whose_turn):
    valid_moves_arr = []
    
    for y in range(6):
        for x in range(6):
            temp_board = deepcopy_2d_list(board_posn) 
            if make_move(x, y, whose_turn, temp_board):
                valid_moves_arr.append([x, y])

    return valid_moves_arr


def auto_pass(whose_turn, current_board_for_trial):
    for y in range(6):
        for x in range(6):
            if make_move(x, y, whose_turn, current_board_for_trial):
                return False
    return True


def victory(current_board):
    black_pieces = sum(row.count(-1) for row in current_board)
    white_pieces = sum(row.count(1) for row in current_board)

    if black_pieces < white_pieces:
        return "White wins!"
    elif black_pieces > white_pieces:
        return "Black wins!"
    else:
        return "Tie!"

def print_board(board):
    for row in board:
        print(" ".join(str(cell) for cell in row))



"""# Initialize board
board = generate_board()
copy_board = deepcopy_2d_list(board)
print("Initial Board:")
print_board(board)
print ("Copy board:")
print_board(copy_board)

# Moves to test
test_moves = [(4, 2, 1), (2, 1, -1), (1, 2, 1), (3, 4, -1)]  # (x, y, whose_turn)

for move in test_moves:
    x, y, turn = move
    print(f"Trying move at ({x}, {y}) for {'White' if turn == 1 else 'Black'}:")
    if make_move(x, y, turn, copy_board):
        print("Move successful!")
    else:
        print("Invalid move!")
    print_board(board)
    print()
    print_board(copy_board)"""
