#include "mlbasics.h"

typedef struct
{
    int input_size;
    int hidden_size;
    int output_size;
    double **w1; // each neuron in a layer connects to multiple in the next hence double pointer
    double *b1;  // each weight is an indepent constt hence one pointer
    double **w2;
    double *b2;
} nn;

double tanh_deriv(double x)
{
    return 1 - pow(tanh(x), 2);
}

nn *initialise_nn(int input_size, int hidden_size, int output_size)
{
    nn *nnpointer = (nn *)malloc(sizeof(nn)); // store the data of a hidden layer

    nnpointer->input_size = input_size; //store the input size in the structure, and similarily store the values to their correspoding variables
    nnpointer->hidden_size = hidden_size;
    nnpointer->output_size = output_size;

    srand(42);

    nnpointer->w1 = (double **)malloc(hidden_size * sizeof(double *)); //allocate memory for input and hidden layer 1
    for (int i = 0; i < hidden_size; i++)
    {
        nnpointer->w1[i] = (double *)malloc(input_size * sizeof(double));
        for (int j = 0; j < input_size; j++)
        {
            nnpointer->w1[i][j] = ((double)rand() / RAND_MAX) * 0.2 - 0.1; //allocate random value between -0.1 and +0.1
        }
    }

    nnpointer->b1 = (double *)calloc(hidden_size, sizeof(double)); //set all biases to 0

    nnpointer->w2 = (double **)(malloc)(output_size * sizeof(double *));//same as above but for hidden to output
    for (int i = 0; i < output_size; i++)
    {
        nnpointer->w2[i] = (double *)(malloc)(hidden_size * sizeof(double));
        for (int j = 0; j < hidden_size; j++)
        {
            nnpointer->w2[i][j] = ((double)rand() / RAND_MAX) * 0.2 - 0.1; //stored in a 2d array, because each hidden layer has different number of neurons and each neuron corresponds to n number of weights
        }
    }

    nnpointer->b2 = (double *)calloc(output_size, sizeof(double));

    return nnpointer;
}

//z1 = (w1x1 + w2x2 + ------ + wnxn)
//a1 = activation(z1);

void forward(nn *nnpointer, double *x, double **a1, double **a2, double **z1, double **z2)//forward prop function
{
    *z1 = (double *)malloc(nnpointer->hidden_size * sizeof(double)); //allocate memory for value of preactivation(z1) and activation(a1) of the function
    *a1 = (double *)malloc(nnpointer->hidden_size * sizeof(double));

    for (int i = 0; i < nnpointer->hidden_size; i++)
    {
        (*z1)[i] = 0; //initialise it to 0
        for (int j = 0; j < nnpointer->input_size; j++)
        {
            (*z1)[i] += nnpointer->w1[i][j] * x[j]; 
        }
        (*z1)[i] += nnpointer->b1[i]; //add the bias
        (*a1)[i] = tanh((*z1)[i]); //use the activation function
    }

    *z2 = (double *)malloc(nnpointer->output_size * sizeof(double));//same as above
    *a2 = (double *)malloc(nnpointer->output_size * sizeof(double));

    for (int i = 0; i < nnpointer->output_size; i++)
    {
        (*z2)[i] = 0;
        for (int j = 0; j < nnpointer->hidden_size; j++)
        {
            (*z2)[i] += nnpointer->w2[i][j] * (*a1)[j];
        }
        (*z2)[i] += nnpointer->b2[i];
        (*a2)[i] = tanh((*z2)[i]);
    }
}

void backward(nn *nnpointer, double *x, double *y, double *a1, double *a2, double *z1, double *z2, double lr) // lr = learning rate
{
    double *dz2 = (double *)malloc(nnpointer->output_size * sizeof(double));//error derivatives, dz1->input to hidden, dz2->hidden to output
    double *dz1 = (double *)malloc(nnpointer->hidden_size * sizeof(double));

    for (int i = 0; i < nnpointer->output_size; i++)
    {
        double error = (a2[i] - y[i]) * 2.0 / nnpointer->output_size; //mean square error derivative
        dz2[i] = error * tanh_deriv(z2[i]); //
    }

    for (int i = 0; i < nnpointer->output_size; i++)
    {
        for (int j = 0; j < nnpointer->hidden_size; j++)
        {
            nnpointer->w2[i][j] -= lr * dz2[i] * a1[j]; //update weights using gradient descent
        }
        nnpointer->b2[i] -= lr * dz2[i]; //update biases
    }

    for (int i = 0; i < nnpointer->hidden_size; i++)
    {
        dz1[i] = 0;
        for (int j = 0; j < nnpointer->output_size; j++)
        {
            dz1[i] += dz2[j] * nnpointer->w2[j][i];
        }
        dz1[i] *= tanh_deriv(z1[i]);
    }

    for (int i = 0; i < nnpointer->hidden_size; i++)//same as above
    {
        for (int j = 0; j < nnpointer->input_size; j++)
        {
            nnpointer->w1[i][j] -= lr * dz1[i] * x[j];
        }
        nnpointer->b1[i] -= lr * dz1[i];
    }

    free(dz2);//free allocated memory
    free(dz1);
}

void train_nn(nn *nnpointer, double **xtrain, double **ytrain, int trainsize, int epochs, double lr)
{
    for (int epoch = 0; epoch < epochs; epoch++)
    {
        double totalloss = 0;//initialise total loss for each epoch

        for (int i = 0; i < trainsize; i++)
        {
            double *x = xtrain[i]; //current training input
            double *y = ytrain[i]; //current training target

            double *a1, *a2, *z1, *z2;

            forward(nnpointer, x, &a1, &a2, &z1, &z2);

            backward(nnpointer, x, y, a1, a2, z1, z2, lr);

            double loss = 0;
            for (int j = 0; j < nnpointer->output_size; j++)
            {
                loss += pow(a2[j] - y[j], 2);//calculated squared error
            }
            totalloss += loss / nnpointer->output_size; //mean squared error

            free(a1);//free allocated memory
            free(a2);
            free(z1);
            free(z2);
        }

        if (epoch % 100 == 0)
        {
            printf("epoch %d - loss: %.4f\n", epoch, totalloss / trainsize);
        }
    }
}

double *predict(nn *nnpointer, double *x)
{
    double *a1, *a2, *z1, *z2;
    forward(nnpointer, x, &a1, &a2, &z1, &z2);//generates predictions

    free(a1);
    free(z1);
    free(z2);

    return a2; //returns output activation
}

void free_nn(nn *nnpointer)
{
    if (!nnpointer) return;
    
    if (nnpointer->w1) {
        for (int i = 0; i < nnpointer->hidden_size; i++) {
            if (nnpointer->w1[i]) free(nnpointer->w1[i]);
        }
        free(nnpointer->w1);
    }

    if (nnpointer->w2) {
        for (int i = 0; i < nnpointer->output_size; i++) {
            if (nnpointer->w2[i]) free(nnpointer->w2[i]);
        }
        free(nnpointer->w2);
    }

    if (nnpointer->b1) free(nnpointer->b1);
    if (nnpointer->b2) free(nnpointer->b2);
    free(nnpointer);
}
