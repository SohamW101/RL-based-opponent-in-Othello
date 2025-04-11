#ifndef NN_H
#define NN_H
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "basic.h"

// typedef struct
// {
//     int input_size;
//     int hidden_size;
//     int output_size;
//     double **w1; // each neuron in a layer connects to multiple in the next hence double pointer
//     double *b1;  // each weight is an indepent constt hence one pointer
//     double **w2;
//     double *b2;
// } nn;

double tanh_deriv(double x);
nn *initialise_nn(int input_size, int hidden_size, int output_size);
void forward(nn *nnpointer, double *x, double **a1, double **a2, double **z1, double **z2);
void backward(nn *nnpointer, double *x, double *y, double *a1, double *a2, double *z1, double *z2, double lr);
void train_nn(nn *nnpointer, double **xtrain, double **ytrain, int trainsize, int epochs, double lr);
double *predict(nn *nnpointer, double *x);
void free_nn(nn *nnpointer);

#endif