#pragma once

#include <stddef.h>

struct LabeledImage {
    float data[28 * 28];
    int label;
};
typedef struct LabeledImage LabeledImage;

struct NeuralNetwork {
    float **layers;
    float **weights;
    float **bias;
    float weights_sizes[6][2];
    int *layers_node_count;
    int layer_count;
};
typedef struct NeuralNetwork NeuralNetwork;

void neural_test();

void setup_network(NeuralNetwork *network, int *layers_node_count,
                   int layer_count, float **weights,
                   float **bias, float **layers);

void guess(float *input, NeuralNetwork *network);

void train(NeuralNetwork *network, float *targets);

void get_input(float *res, char *image_path);

LabeledImage *load_dataset(char *path, size_t *len);

void get_target_array(float arr[10], LabeledImage img);
