#include "err.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../utils.h"
#include "matrix.h"
#include "neural-net.h"

int main(int argc, char **argv) {
    if (argc < 5 || argc > 9) {
        errx(1, "\nUsage:\n"
                "./neural image_path nb_input nb_hidden1 ... nb_hidden(n) "
                "nb_output\n\n"
                "Maximum of 5 hidden layers");
    }

    test_matrix();

    int layer_count = argc - 2;
    int layers_node_count[6];

    char *image_path = argv[1];
    size_t len_dataset;
    LabeledImage *dataset = load_dataset(image_path, &len_dataset);

    char *ptr;
    for (int i = 2; i < argc; i++) {
        layers_node_count[i - 2] = strtol(argv[i], &ptr, 10);
    }

    // Initialize randomizer
    srand((unsigned int)time(NULL));

    // Build neural network
    float weights_sizes[6][2];

    float **weights;
    weights = malloc((layer_count - 1) * sizeof(float *));

    float **bias;
    bias = malloc((layer_count - 1) * sizeof(float *));

    float **layers;
    layers = malloc(layer_count * sizeof(float *));

    NeuralNetwork network;

    setup_network(&network, layers_node_count, layer_count, weights, bias,
                  layers);

    for (int k = 0; k < 1; k++) {
        int total = 0;
        int correct = 0;
        for (size_t i = 0; i < len_dataset; i++) {

            int dataset_index = rand() % len_dataset;

            guess(dataset[dataset_index].data, &network);

            size_t output_size = layers_node_count[layer_count - 1];
            float *output_layer = layers[layer_count - 1];

            size_t max_index = 0;
            for (size_t j = 0; j < output_size; j++) {
                if (output_layer[j] > output_layer[max_index]) {
                    max_index = j;
                }
            }
            total++;
            correct += (int)max_index == dataset[dataset_index].label;

            train(&network, &dataset[dataset_index]);
        }
        if (k % 10 == 0) {
            printf("[EPOCH %d] %d correct out of %d (%.2f%%)\n", k, correct,
                   total, (float)correct * 100 / (float)total);
        }
    }

    for (int i = 0; i < layer_count - 1; i++) {
        free(weights[i]);
        free(bias[i]);
        free(layers[i + 1]);
    }

    free(weights);
    free(bias);
    free(layers);
    free(dataset);
}

void setup_network(NeuralNetwork *network, int *layers_node_count,
                   int layer_count, float **weights, float **bias,
                   float **layers) {
    for (int i = 0; i < layer_count - 1; i++) {
        network->weights_sizes[i][0] = layers_node_count[i + 1];
        network->weights_sizes[i][1] = layers_node_count[i];
        weights[i] = malloc(network->weights_sizes[i][0] *
                            network->weights_sizes[i][1] * sizeof(float));
        bias[i] = calloc(layers_node_count[i + 1], sizeof(float));
        layers[i + 1] = malloc(layers_node_count[i + 1] * sizeof(float));
        mat_randomize(weights[i], network->weights_sizes[i][0] *
                                      network->weights_sizes[i][1]);
    }

    network->layers = layers;
    network->bias = bias;
    network->weights = weights;
    network->layer_count = layer_count;
    network->layers_node_count = layers_node_count;

    return;

    FILE *ptr;
    char s[50];
    ptr = fopen("save.neural", "r");

    if (NULL == ptr) {
        printf("file can't be opened \n");
    } else {
        int current_layer = 0;
        while (fgets(s, 50, ptr) != NULL && current_layer < layer_count) {
            if (strcmp(s, "--\n")) {
                current_layer++;
            }
        }
        fclose(ptr);
    }
}

void guess(float *input, NeuralNetwork *network) {
    network->layers[0] = input;

    // FeedForward
    for (int j = 0; j < network->layer_count - 1; j++) {
        mat_multiply(network->layers[j + 1], network->weights[j],
                     network->layers[j], network->layers_node_count[j + 1],
                     network->layers_node_count[j], 1);
        mat_add(network->layers[j + 1], network->layers[j + 1],
                network->bias[j], network->layers_node_count[j + 1], 1);

        // Sigmoid for hidden layers, Softmax for output layer
        if (j + 1 != network->layer_count - 1) {
            mat_apply_sigmoid(network->layers[j + 1],
                              network->layers_node_count[j + 1]);
        } else {
            mat_apply_softmax(network->layers[j + 1],
                              network->layers_node_count[j + 1]);
        }
    }
}

void train(NeuralNetwork *network, LabeledImage *image) {
    float targets[10];
    get_target_array(targets, *image);

    float learning_rate = 0.03;

    size_t output_size = network->layers_node_count[network->layer_count - 1];
    float *output_layer = network->layers[network->layer_count - 1];
    float *errors = malloc(output_size * sizeof(float));
    mat_substract(errors, targets, output_layer, output_size, 1);

    // Back Propagate
    float *gradients = malloc(output_size * sizeof(float));
    mat_copy(output_layer, gradients, output_size);
    mat_apply_dsoftmax(gradients, output_size);
    mat_multiply_hadamard(gradients, gradients, errors, output_size, 1);
    mat_multiply_scalar(gradients, gradients, learning_rate, output_size, 1);

    float *hidden_t = malloc(network->layers_node_count[1] * sizeof(float));
    float *weights_ho_deltas =
        malloc(network->weights_sizes[1][0] * network->weights_sizes[1][1] *
               sizeof(float));
    mat_transpose(hidden_t, network->layers[1], network->layers_node_count[1],
                  1);
    mat_multiply(weights_ho_deltas, gradients, hidden_t,
                 network->layers_node_count[2], 1,
                 network->layers_node_count[1]);

    mat_add(network->weights[1], network->weights[1], weights_ho_deltas,
            network->weights_sizes[1][0], network->weights_sizes[1][1]);
    mat_add(network->bias[1], network->bias[1], gradients,
            network->layers_node_count[2], 1);

    for (int j = network->layer_count - 2; j > 0; j--) {
        float *weight =
            malloc(network->weights_sizes[j - 1][0] *
                   network->weights_sizes[j - 1][1] * sizeof(float));
        float *layer_errors =
            malloc(network->layers_node_count[j] * sizeof(float));
        float *layer_gradient =
            malloc(network->layers_node_count[j] * sizeof(float));
        mat_transpose(weight, network->weights[j - 1],
                      network->weights_sizes[j - 1][0],
                      network->weights_sizes[j - 1][1]);
        mat_multiply(layer_errors, weight, errors, network->weights_sizes[j][1],
                     network->weights_sizes[j][0], 1);
        mat_copy(network->layers[j], layer_gradient,
                 network->layers_node_count[j]);
        mat_apply_dsigmoid(layer_gradient, network->layers_node_count[j]);
        mat_multiply_hadamard(layer_gradient, layer_gradient, layer_errors,
                              network->layers_node_count[j], 1);
        mat_multiply_scalar(layer_gradient, layer_gradient, learning_rate,
                            network->layers_node_count[j], 1);

        float *previous_layer_T =
            malloc(network->layers_node_count[j - 1] * sizeof(float));
        float *weight_deltas =
            malloc(network->weights_sizes[j - 1][0] *
                   network->weights_sizes[j - 1][1] * sizeof(float));
        mat_transpose(previous_layer_T, network->layers[j - 1],
                      network->layers_node_count[j - 1], 1);
        mat_multiply(weight_deltas, layer_gradient, previous_layer_T,
                     network->layers_node_count[j], 1,
                     network->layers_node_count[j - 1]);

        mat_add(network->weights[j - 1], network->weights[j - 1], weight_deltas,
                network->weights_sizes[j - 1][0],
                network->weights_sizes[j - 1][1]);
        mat_add(network->bias[j - 1], network->bias[j - 1], layer_gradient,
                network->layers_node_count[j], 1);

        free(weight);
        free(weight_deltas);
        free(layer_gradient);
        free(layer_errors);
        free(previous_layer_T);
    }

    free(errors);
    free(gradients);
    free(weights_ho_deltas);
    free(hidden_t);
}

void get_target_array(float arr[10], LabeledImage img) {
    for (int i = 0; i < 10; i++) {
        arr[i] = i == img.label ? 1 : 0;
    }
}

LabeledImage *load_dataset(char *path, size_t *len_d) {
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    size_t len = 0;

    if (!d)
        errx(1, "Could not load dataset at '%s'", path);

    while ((dir = readdir(d)) != NULL) {
        if (!strcmp(strrchr(dir->d_name, '\0') - 4, ".bmp")) {
            len++;
        }
    }
    closedir(d);

    struct LabeledImage *dataset = malloc(len * sizeof(struct LabeledImage));
    d = opendir(path);

    size_t i = 0;
    while ((dir = readdir(d)) != NULL) {
        if (!strcmp(strrchr(dir->d_name, '\0') - 4, ".bmp")) {
            char *tmppath =
                malloc((strlen(path) + strlen(dir->d_name) + 5) * sizeof(char));
            get_input(dataset[i].data,
                      strcat(strcpy(tmppath, path), dir->d_name));
            dataset[i].label = dir->d_name[0] - 48;
            free(tmppath);
            i++;
        }
    }
    closedir(d);

    printf("Loaded %zu images\n", len);
    (*len_d) = len;
    return dataset;
}

void get_input(float *res, char *image_path) {
    SDL_Surface *surface;
    surface = IMG_Load(image_path);
    if (!surface) {
        errx(1, "Could not load image (%s)", image_path);
    }
    int w, h;
    w = surface->w;
    h = surface->h;

    Uint8 r, g, b;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            GetPixelColor(surface, x, y, &r, &g, &b);
            unsigned char average = (r + g + b) / 3;
            res[x * 28 + y] = (float)average / 255;
        }
    }

    /*for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float a = res[x * 28 + y] * 255;
            char c;
            if (a > 240)
                c = ' ';
            else if (a > 200)
                c = '.';
            else if (a > 150)
                c = ':';
            else if (a > 120)
                c = '=';
            else if (a > 100)
                c = '+';
            else if (a > 90)
                c = '*';
            else if (a > 70)
                c = '#';
            else if (a > 50)
                c = '%';
            else
                c = '@';
            printf("%c", c);
        }
        printf("\n");
    }*/
    SDL_FreeSurface(surface);
}
