#pragma once

#include <stdio.h>
#include <stddef.h>
#include "matrix.h"

#define szt size_t

typedef struct LabeledImage {
    double data[28 * 28];
    int label;
} LabeledImage;

enum ActivationFunction {
    ReLU,
    Sigmoid,
    SoftMax
};

typedef struct Layer {
    size_t size;
    enum ActivationFunction activation;
    Matrix A;
    Matrix Z;
    Matrix W;
    Matrix B;
} Layer;

typedef struct Updates {
    Matrix dZ1;
    Matrix dZ2;
    Matrix dW1;
    Matrix dW2;
    Matrix mW1;
    Matrix mW2;
    Matrix dB1;
    Matrix dB2;
    Matrix mB1;
    Matrix mB2;
    double current_learning_rate;
    szt iterations;
} Updates;

typedef struct NeuralNetwork {
    Layer* layers;
    Updates updates;
    size_t layer_count;
    double learning_rate;
    double decay;
    double momentum;
    size_t batch_size;
    FILE* csv;
} NeuralNetwork;

void serialize_layer(Buffer* buf, Layer* layer);
NeuralNetwork new_network(double learning_rate);
Layer new_layer(enum ActivationFunction activation, size_t num_nodes, size_t prev_num_nodes, size_t batch_size);
void free_network(NeuralNetwork* nn);
void free_layer(Layer* layer);
void print_stat(NeuralNetwork* nn, double error);

int guess_xor(char *input);

int main_xor(char *path);

void save_network(NeuralNetwork *nn);

void load_network(NeuralNetwork *nn, char *path);

void train_network(NeuralNetwork *neural_net, char* dataset_path, size_t iterations);

void setup_network(NeuralNetwork *nn, int *layers_node_count, int batch_size,
                   float learning_rate, int layer_count);

void load_image(double *res, char *image_path);

void print_pixel(double *img, int w, int h);

LabeledImage *load_dataset(char *path, size_t *len);

