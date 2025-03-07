class Cell:
    def __init__(self, x, y, status=0):
        self.x = x
        self.y = y
        self.status = status

# +1 for white, -1 for black and 0 for blank

def generate_board():   
    board = []
    for y in range(6):
        rows = []
        for x in range(6):
            cell = Cell(x, y)
            if (x == 2 and y == 3) or (x == 3 and y == 2):
                cell.status = -1
            elif (x == 2 and y== 2) or (x == 3 and y == 3):
                cell.status = 1
            rows.append(cell)
        board.append(rows)
    return board


# Game Handling

# basics
def check_for_blank(x, y, current_board):
    color = current_board[y][x].status
    if color == 0:
        return True
    return False


def toggle(whose_turn):     #whose_turn is an int: 1 or -1
    if whose_turn == 1:
        return -1
    else:
        return 1
    

# check is there any valid move left -- store those valid moves

def flip_pieces(x, y, dx, dy, whose_turn, current_board):      #whose_turn is an int: 1 or -1
    color = whose_turn
    color_to_trap = toggle(color)

    new_x, new_y = x + dx, y + dy
    flip_positions = []

    while 0 <= new_x < 6 and 0 <= new_y < 6:
        next_cell_color = current_board[new_y][new_x].status

        if next_cell_color == color_to_trap:
            flip_positions.append((new_x, new_y))
            new_x += dx
            new_y += dy

        elif next_cell_color == color:
            if not flip_positions:  # No opponent pieces to flip
                return False

            # Flip all stored positions
            for fx, fy in flip_positions:
                current_board[fy][fx].status = color
            return True

        else:  # Empty space
            return False

    return False  # No valid move found


def make_move(x, y, whose_turn, current_board):
    if check_for_blank(x, y, current_board):
        directions = [(0, 1), (0, -1), (1, 0), (-1, 0), (1, 1), (-1, 1), (1, -1), (-1, -1)]
        flipped = False

        for dx, dy in directions:
            if flip_pieces(x, y, dx, dy, whose_turn, current_board):
                flipped = True

        if flipped:
            current_board[y][x].status = whose_turn  # Place the new piece
            return True

    return False


# Check for passes
def auto_pass(whose_turn, current_board_for_trial):
    for y in range(6):
        for x in range(6):
            if make_move(x, y, whose_turn, current_board_for_trial):
                return False
    return True


# Victory
def victory(current_board):
    black_pieces = 0
    white_pieces = 0

    for y in range(6):
        for x in range(6):
            if current_board[y][x].status == -1:
                black_pieces += 1
            elif current_board[y][x].status == 1:
                white_pieces += 1

    if black_pieces < white_pieces:
        return "White wins!"
    elif black_pieces > white_pieces:
        return "Black wins!"
    else:
        return "Tie!"