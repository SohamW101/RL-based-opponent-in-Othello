def save_weights(W1, b1, W2, b2, filename="weights.txt"):
    with open(filename, "w") as f:
        for row in W1:
            f.write(" ".join(map(str, row)) + "\n")  # Save W1 (36 × 24)
        f.write(" ".join(map(str, b1)) + "\n")  # Save b1 (24,)
        for row in W2:
            f.write(" ".join(map(str, row)) + "\n")  # Save W2 (24 × 36)
        f.write(" ".join(map(str, b2)) + "\n")  # Save b2 (36,)

def load_weights(filename="weights.txt"):
    with open(filename, "r") as f:
        lines = f.readlines()
    
    W1 = [list(map(float, lines[i].split())) for i in range(36)]  # W1: 36 rows
    b1 = list(map(float, lines[36].split()))  # b1: 1 row
    W2 = [list(map(float, lines[i].split())) for i in range(37, 61)]  # W2: 24 rows
    b2 = list(map(float, lines[61].split()))  # b2: 1 row
    
    return W1, b1, W2, b2
