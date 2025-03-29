#ifndef ML_BASICS_H
#define ML_BASICS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Sigmoid activation function
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x)); // Maps any input to a value between 0 and 1
}

// Derivative of the sigmoid function
double dsigmoid(double x) {
    return x * (1.0 - x); // Used in backpropagation for gradient calculation
}

// ReLU activation function
double relu(double x) {
    return (x > 0) ? x : 0; // Returns x if x > 0, otherwise 0
}

// Derivative of the ReLU function
double drelu(double x) {
    return (x > 0) ? 1.0 : 0; // Returns 1 if x > 0, otherwise 0
}

// Initialize weights with random values between -1 and 1
double init_weight() {
    return ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0; // Random weight between -1 and 1
}

// Initialize biases with random values between -1 and 1
double init_bias() {
    return ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0; // Random bias between -1 and 1
}

// Shuffle an array (used for shuffling training data)
void shuffle(int *array, size_t n) {
    if (n > 1) {
        for (size_t i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1); // Random index for shuffling
            int t = array[j]; // Swap elements
            array[j] = array[i];
            array[i] = t;
        }
    }
}

// Mean Squared Error (MSE) loss function
double mse_loss(double predicted, double target) {
    return 0.5 * pow(target - predicted, 2); // MSE for a single data point
}

// Derivative of the MSE loss function
double dmse_loss(double predicted, double target) {
    return predicted - target; // Derivative of MSE w.r.t. predicted value
}

// Softmax function (for multi-class classification)
void softmax(double *input, size_t n, double *output) {
    double max = input[0];
    for (size_t i = 1; i < n; i++) {
        if (input[i] > max) {
            max = input[i]; // Find the maximum value for numerical stability
        }
    }

    double sum = 0.0;
    for (size_t i = 0; i < n; i++) {
        output[i] = exp(input[i] - max); // Subtract max for numerical stability
        sum += output[i];
    }

    for (size_t i = 0; i < n; i++) {
        output[i] /= sum; // Normalize to get probabilities
    }
}

// Cross-Entropy Loss (for classification tasks)
double cross_entropy_loss(double *predicted, double *target, size_t n) {
    double loss = 0.0;
    for (size_t i = 0; i < n; i++) {
        loss += target[i] * log(predicted[i]); // Cross-entropy formula
    }
    return -loss; // Return negative loss
}

// Derivative of Cross-Entropy Loss with Softmax
void dcross_entropy_softmax(double *predicted, double *target, size_t n, double *output) {
    for (size_t i = 0; i < n; i++) {
        output[i] = predicted[i] - target[i]; // Derivative of cross-entropy with softmax
    }
}

// Matrix multiplication (for neural network layers)
void matmul(double *A, double *B, double *C, size_t m, size_t n, size_t p) {
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < p; j++) {
            C[i * p + j] = 0.0;
            for (size_t k = 0; k < n; k++) {
                C[i * p + j] += A[i * n + k] * B[k * p + j]; // Matrix multiplication
            }
        }
    }
}

// Add bias to a layer's output
void add_bias(double *output, double *bias, size_t n) {
    for (size_t i = 0; i < n; i++) {
        output[i] += bias[i]; // Add bias to each neuron's output
    }
}

// Initialize a matrix with random values
void init_matrix(double *matrix, size_t rows, size_t cols) {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            matrix[i * cols + j] = init_weight(); // Initialize with random weights
        }
    }
}

// Print a matrix (for debugging)
void print_matrix(double *matrix, size_t rows, size_t cols) {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            printf("%f ", matrix[i * cols + j]); // Print each element
        }
        printf("\n");
    }
}

// Forward propagation for a single layer
void forward_propagate(double *input, double *weights, double *biases, double *output, size_t input_size, size_t output_size, double (*activation)(double)) {
    matmul(input, weights, output, 1, input_size, output_size); // Matrix multiplication
    add_bias(output, biases, output_size); // Add biases
    for (size_t i = 0; i < output_size; i++) {
        output[i] = activation(output[i]); // Apply activation function
    }
}

// Backward propagation for a single layer
void backward_propagate(double *input, double *weights, double *biases, double *output, double *error, size_t input_size, size_t output_size, double (*dactivation)(double)) {
    double *delta = (double *)malloc(output_size * sizeof(double)); // Allocate memory for delta
    for (size_t i = 0; i < output_size; i++) {
        delta[i] = error[i] * dactivation(output[i]); // Compute delta
    }

    double *gradient = (double *)malloc(input_size * output_size * sizeof(double)); // Allocate memory for gradient
    matmul(input, delta, gradient, input_size, 1, output_size); // Compute gradient

    // Update weights and biases
    for (size_t i = 0; i < input_size; i++) {
        for (size_t j = 0; j < output_size; j++) {
            weights[i * output_size + j] -= gradient[i * output_size + j]; // Update weights
        }
    }
    for (size_t i = 0; i < output_size; i++) {
        biases[i] -= delta[i]; // Update biases
    }

    free(delta); // Free allocated memory
    free(gradient);
}

// Update weights using gradient descent
void update_weights(double *weights, double *gradient, size_t rows, size_t cols, double learning_rate) {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            weights[i * cols + j] -= learning_rate * gradient[i * cols + j]; // Update weights
        }
    }
}

// Update biases using gradient descent
void update_biases(double *biases, double *delta, size_t n, double learning_rate) {
    for (size_t i = 0; i < n; i++) {
        biases[i] -= learning_rate * delta[i]; // Update biases
    }
}

// Compute gradient for a layer
void compute_gradient(double *input, double *delta, double *gradient, size_t input_size, size_t output_size) {
    matmul(input, delta, gradient, input_size, 1, output_size); // Compute gradient
}

// Compute error for the output layer
void compute_output_error(double *output, double *target, double *error, size_t n) {
    for (size_t i = 0; i < n; i++) {
        error[i] = output[i] - target[i]; // Compute error
    }
}

// Compute error for a hidden layer
void compute_hidden_error(double *weights, double *error_next, double *delta_next, double *error, size_t input_size, size_t output_size) {
    matmul(weights, error_next, error, input_size, output_size, 1); // Compute error
    for (size_t i = 0; i < input_size; i++) {
        error[i] *= delta_next[i]; // Multiply by delta
    }
}

#endif // ML_BASICS_H