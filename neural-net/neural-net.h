#pragma once

#include <stddef.h>

struct LabeledImage {
    float data[28 * 28];
    int label;
};
typedef struct LabeledImage LabeledImage;

struct Layer {
    float *data;
    float *z;
    float *dz;
    char *disabled;
    size_t size;
};
typedef struct Layer Layer;

struct NeuralNetwork {
    Layer *layers;
    float **weights;
    float **bias;
    int weights_sizes[6][2];
    int *layers_node_count;
    int layer_count;
    int batch_size;
    float learning_rate;
    float loss;
};
typedef struct NeuralNetwork NeuralNetwork;

int guess_xor(char **input);

int main_xor(char *path);

void save_network(NeuralNetwork *nn);

void load_network(NeuralNetwork *nn, char *path);

void setup_network(NeuralNetwork *nn, int *layers_node_count, int batch_size,
                   float learning_rate, int layer_count);

void get_input(float *res, char *image_path);

LabeledImage *load_dataset(char *path, size_t *len);
