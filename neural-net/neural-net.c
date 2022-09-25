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

    int layer_count = argc - 1;
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

    // setup_network(layers_node_count, layer_count);

    // Build neural network
    float learning_rate = 0.001;
    float weights_sizes[3][2] = {
        {layers_node_count[1], layers_node_count[0]},
        {layers_node_count[2], layers_node_count[1]},
    };

    size_t len_w_ih = weights_sizes[0][0] * weights_sizes[0][1];
    float *weights_ih = malloc(len_w_ih * sizeof(float));

    size_t len_w_ho = weights_sizes[1][0] * weights_sizes[1][1];
    float *weights_ho = malloc(len_w_ho * sizeof(float));

    float *bias_h = calloc(layers_node_count[1], sizeof(float));
    float *bias_o = calloc(layers_node_count[2], sizeof(float));

    float *hidden = malloc(layers_node_count[1] * sizeof(float));
    float *outputs = malloc(layers_node_count[2] * sizeof(float));

    mat_randomize(weights_ih, len_w_ih);
    mat_randomize(weights_ho, len_w_ho);

    for (int k = 0; k < 5000; k++) {
        int total = 0;
        int correct = 0;
        for (size_t i = 0; i < len_dataset; i++) {

            int dataset_index = rand() % len_dataset;
            float *input = dataset[dataset_index].data;

            // FeedForward
            mat_multiply(hidden, weights_ih, input, layers_node_count[1],
                         layers_node_count[0], 1);
            mat_add(hidden, hidden, bias_h, layers_node_count[1], 1);
            mat_apply_sigmoid(hidden, layers_node_count[1]);

            mat_multiply(outputs, weights_ho, hidden, layers_node_count[2],
                         layers_node_count[1], 1);
            mat_add(outputs, outputs, bias_o, layers_node_count[2], 1);
            mat_apply_softmax(outputs, layers_node_count[2]);

            size_t max_index = 0;
            for (size_t i = 0; i < layers_node_count[2]; i++) {
                if (outputs[i] > outputs[max_index]) {
                    max_index = i;
                }
            }
            total++;
            correct += max_index == dataset[dataset_index].label;

            float targets[10];
            get_target_array(targets, dataset[dataset_index]);

            float *errors = malloc(layers_node_count[2] * sizeof(float));
            mat_substract(errors, targets, outputs, layers_node_count[2], 1);

            
            // Back Propagate
            float *gradients = malloc(layers_node_count[2] * sizeof(float));
            mat_copy(outputs, gradients, layers_node_count[2]);
            mat_apply_dsoftmax(gradients, layers_node_count[2]);
            mat_multiply_hadamard(gradients, gradients, errors,
                                  layers_node_count[2], 1);
            mat_multiply_scalar(gradients, gradients, learning_rate,
                                layers_node_count[2], 1);

            float *hidden_t = malloc(layers_node_count[1] * sizeof(float));
            mat_transpose(hidden_t, hidden, layers_node_count[1], 1);
            float *weights_ho_deltas = malloc(len_w_ho * sizeof(float));
            mat_multiply(weights_ho_deltas, gradients, hidden_t,
                         layers_node_count[2], 1, layers_node_count[1]);

            mat_add(weights_ho, weights_ho, weights_ho_deltas,
                    weights_sizes[1][0], weights_sizes[1][1]);
            mat_add(bias_o, bias_o, gradients, layers_node_count[2], 1);

            float *weights_ho_t = malloc(len_w_ho * sizeof(float));
            mat_transpose(weights_ho_t, weights_ho, weights_sizes[1][0],
                          weights_sizes[1][1]);
            float *hidden_errors = malloc(layers_node_count[1] * sizeof(float));
            mat_multiply(hidden_errors, weights_ho_t, errors,
                         weights_sizes[1][1], weights_sizes[1][0], 1);
            float *hidden_gradient =
                malloc(layers_node_count[1] * sizeof(float));
            mat_copy(hidden, hidden_gradient, layers_node_count[1]);
            mat_apply_dsigmoid(hidden_gradient, layers_node_count[1]);
            mat_multiply_hadamard(hidden_gradient, hidden_gradient,
                                  hidden_errors, layers_node_count[1], 1);
            mat_multiply_scalar(hidden_gradient, hidden_gradient, learning_rate,
                                layers_node_count[1], 1);

            float *input_t = malloc(layers_node_count[0] * sizeof(float));
            mat_transpose(input_t, input, layers_node_count[0], 1);
            float *weights_ih_deltas = malloc(len_w_ih * sizeof(float));
            mat_multiply(weights_ih_deltas, hidden_gradient, input_t,
                         layers_node_count[1], 1, layers_node_count[0]);

            mat_add(weights_ih, weights_ih, weights_ih_deltas,
                    weights_sizes[0][0], weights_sizes[0][1]);
            mat_add(bias_h, bias_h, hidden_gradient, layers_node_count[1], 1);

            free(errors);
            free(weights_ho_t);
            free(gradients);
            free(hidden_gradient);
            free(weights_ho_deltas);
            free(weights_ih_deltas);
            free(hidden_errors);
            free(hidden_t);
            free(input_t);
        }
        if (k % 10 == 0) {
            printf("[EPOCH %d] %d correct out of %d (%.2f%%)\n", k, correct,
                   total, (float)correct * 100 / (float)total);
        }
    }

    free(hidden);
    free(outputs);
    free(weights_ih);
    free(weights_ho);
    free(bias_h);
    free(bias_o);
    free(dataset);
}

void setup_network(int *layers_node_count, int layer_count) {
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
