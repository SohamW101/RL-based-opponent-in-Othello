def save_weights(W1, b1, W2, b2, filename="weights.txt"):
    with open(filename, "w") as f:
        for row in W1:
            f.write(" ".join(map(str, row)) + "\n")  # Save W1
        f.write(" ".join(map(str, b1)) + "\n")  # Save b1
        for row in W2:
            f.write(" ".join(map(str, row)) + "\n")  
        f.write(" ".join(map(str, b2)) + "\n")  

def load_weights(filename="weights.txt"):
    with open(filename, "r") as f:
        lines = f.readlines()
    
    W1 = [list(map(float, line.split())) for line in lines[:44]]  # First 44 lines for W1
    b1 = list(map(float, lines[44].split()))  # Next line for b1
    W2 = [list(map(float, line.split())) for line in lines[45:81]]  # Next 36 lines for W2
    b2 = list(map(float, lines[81].split()))  # Last line for b2
    
    return W1, b1, W2, b2
