import random
import math

def tanh(x):
    x = max(-700, min(700, x))  # Prevent overflow
    return (math.exp(x) - math.exp(-x)) / (math.exp(x) + math.exp(-x))


def tanh_derivative(x):
    return 1 - tanh(x) ** 2


# Initialize weights and biases
def initialize_nn(input_size, hidden_size, output_size):
    random.seed(42)  # For reproducibility

    W1 = []
    for i in range(hidden_size):
        row = []
        for j in range(input_size):
            row.append(random.uniform(-0.1, 0.1))
        W1.append(row)

    b1 = []
    for i in range(hidden_size):
        b1.append(0)

    W2 = []
    for i in range(output_size):
        row = []
        for j in range(hidden_size):
            row.append(random.uniform(-0.1, 0.1))
        W2.append(row)

    b2 = []
    for i in range(output_size):
        b2.append(0)

    return W1, b1, W2, b2

def forward(X, W1, b1, W2, b2):
    Z1 = []
    for i in range(len(W1)):
        sum_val = 0
        for j in range(len(X)):
            sum_val += W1[i][j] * X[j]
        sum_val += b1[i]
        Z1.append(sum_val)

    A1 = []
    for i in range(len(Z1)):
        A1.append(tanh(Z1[i]))

    Z2 = []
    for i in range(len(W2)):
        sum_val = 0
        for j in range(len(A1)):
            sum_val += W2[i][j] * A1[j]
        sum_val += b2[i]
        Z2.append(sum_val)

    A2 = []
    for i in range(len(Z2)):
        A2.append(tanh(Z2[i]))

    return A1, A2, Z1, Z2

def backward(X, Y, A1, A2, Z1, Z2, W1, b1, W2, b2, learning_rate):
    output_size = len(Y)
    hidden_size = len(W1)

    # Compute output layer gradient
    dA2 = []
    for i in range(output_size):
        dA2.append((A2[i] - Y[i]) * 2 / output_size)

    dZ2 = []
    for i in range(output_size):
        dZ2.append(dA2[i] * tanh_derivative(Z2[i]))

    # Compute weight and bias gradients for output layer
    dW2 = []
    for i in range(output_size):
        row = []
        for j in range(hidden_size):
            row.append(dZ2[i] * A1[j])
        dW2.append(row)

    db2 = []
    for i in range(output_size):
        db2.append(dZ2[i])

    # Compute hidden layer gradient
    dA1 = []
    for j in range(hidden_size):
        sum_val = 0
        for i in range(output_size):
            sum_val += W2[i][j] * dZ2[i]
        dA1.append(sum_val)

    dZ1 = []
    for j in range(hidden_size):
        dZ1.append(dA1[j] * tanh_derivative(Z1[j]))

    # Compute weight and bias gradients for hidden layer
    dW1 = []
    for i in range(hidden_size):
        row = []
        for j in range(len(X)):
            row.append(dZ1[i] * X[j])
        dW1.append(row)

    db1 = []
    for i in range(hidden_size):
        db1.append(dZ1[i])

    # Update weights and biases using gradient descent
    for i in range(output_size):
        for j in range(hidden_size):
            W2[i][j] -= learning_rate * dW2[i][j]
        b2[i] -= learning_rate * db2[i]

    for i in range(hidden_size):
        for j in range(len(X)):
            W1[i][j] -= learning_rate * dW1[i][j]
        b1[i] -= learning_rate * db1[i]

    return W1, b1, W2, b2



def train_nn(X_train, Y_train, W1, b1, W2, b2, epochs = 1000, learning_rate = 0.1):
    for epoch in range(epochs):
        loss = 0
        for i in range(len(X_train)):  
            X = X_train[i]
            Y = Y_train[i]

            A1, A2, Z1, Z2 = forward(X, W1, b1, W2, b2)
            W1, b1, W2, b2 = backward(X, Y, A1, A2, Z1, Z2, W1, b1, W2, b2, learning_rate)

            loss_val = 0
            for j in range(len(Y)):
                loss_val += (A2[j] - Y[j]) ** 2
            loss += loss_val / len(Y)

        if epoch % 100 == 0:
            print("Epoch", epoch, "Loss =", loss / len(X_train))

    return W1, b1, W2, b2


def predict(X, W1, b1, W2, b2):
    _, A2, _, _ = forward(X, W1, b1, W2, b2)    
    return A2



# Example usage
# XOR dataset
X_train = [[0,0], [0,1], [1,0], [1,1]]
Y_train = [[0], [1], [1], [0]]

# Initialize network
W1, b1, W2, b2 = initialize_nn(2, 4, 1)

# Train
train_nn(X_train, Y_train, W1, b1, W2, b2, 1000, 0.1)

# Test
for x in X_train:
    print(f"{x} => {predict(x, W1, b1, W2, b2)}")
