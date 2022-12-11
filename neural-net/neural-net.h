#pragma once

#include "matrix.h"
#include <SDL2/SDL.h>
#include <stddef.h>
#include <stdio.h>

#define szt size_t

typedef struct LabeledImage {
    double data[28 * 28];
    int label;
} LabeledImage;

enum ActivationFunction { ReLU, Sigmoid, SoftMax };

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
    Layer *layers;
    Updates updates;
    size_t layer_count;
    double learning_rate;
    double decay;
    double momentum;
    size_t batch_size;
    FILE *csv;
} NeuralNetwork;

void serialize_layer(Buffer *buf, Layer *layer);
NeuralNetwork new_network(double learning_rate);
Layer new_layer(enum ActivationFunction activation, size_t num_nodes,
                size_t prev_num_nodes, size_t batch_size);
void free_network(NeuralNetwork *nn);
void free_layer(Layer *layer);
void print_stat(NeuralNetwork *nn, double error, double validation);

double gauss_noise();
void cmd_train(int argc, char **argv);
void cmd_guess(int argc, char **argv);
void cmd_test(int argc, char **argv);

int guess_xor(char *input);

int main_xor(char *path);

char recognize_digit(SDL_Surface *surf);

void save_network(NeuralNetwork *nn);

void load_network(NeuralNetwork *nn, char *path);

void train_network(NeuralNetwork *neural_net, char *dataset_path,
                   size_t iterations);

void setup_network(NeuralNetwork *nn, int *layers_node_count, int batch_size,
                   float learning_rate, int layer_count);

void load_from_surface(double *res, SDL_Surface *surface);
void load_image(double *res, char *image_path);

void print_pixel(double *img, int w, int h);

LabeledImage *load_all_cutter_set(char *dataset_path, szt *len_d);
LabeledImage *load_cutter_set(char *path, szt *len_d);
LabeledImage *load_dataset(char *path, size_t *len);
