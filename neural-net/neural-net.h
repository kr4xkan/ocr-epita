#pragma once

#include <stddef.h>
#include "matrix.h"

#define szt size_t

struct LabeledImage {
    float data[28 * 28];
    int label;
};
typedef struct LabeledImage LabeledImage;

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

typedef struct NeuralNetwork {
    Layer* layers;
    size_t layer_count;
    float learning_rate;
} NeuralNetwork;

NeuralNetwork new_network(int argc, char** argv);
Layer new_layer(enum ActivationFunction activation, size_t num_nodes, size_t prev_num_nodes);

int guess_xor(char *input);

int main_xor(char *path);

void save_network(NeuralNetwork *nn);

void load_network(NeuralNetwork *nn, char *path);

void setup_network(NeuralNetwork *nn, int *layers_node_count, int batch_size,
                   float learning_rate, int layer_count);

void load_image(double *res, char *image_path);

void print_pixel(double *img, int w, int h);

LabeledImage *load_dataset(char *path, size_t *len);

