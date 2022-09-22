#include "err.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../utils.h"
#include "matrix.h"
#include "neural-net.h"

SDL_Surface *surface;

void test_matrices();

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
    char *ptr;
    for (int i = 2; i < argc; i++) {
        layers_node_count[i - 2] = strtol(argv[i], &ptr, 10);
    }

    // Initialize randomizer
    srand((unsigned int)time(NULL));

    // Build neural network
    float *weights_ih =
        malloc(layers_node_count[1] * layers_node_count[0] * sizeof(float));
    size_t len_w_ih = layers_node_count[1] * layers_node_count[0];

    float *weights_ho =
        malloc(layers_node_count[2] * layers_node_count[1] * sizeof(float));
    size_t len_w_ho = layers_node_count[2] * layers_node_count[1];

    mat_randomize(weights_ih, len_w_ih);
    mat_randomize(weights_ho, len_w_ho);

    float *bias_h = calloc(layers_node_count[1], sizeof(float));
    float *bias_o = calloc(layers_node_count[2], sizeof(float));

    float *input = malloc(28 * 28 * sizeof(float));
    get_input(input, image_path);
    float *hidden = malloc(layers_node_count[1] * sizeof(float));
    mat_multiply(hidden, weights_ih, input, layers_node_count[1],
                 layers_node_count[0], 1);
    mat_add(hidden, hidden, bias_h, layers_node_count[1], 1);
    mat_apply_sigmoid(hidden, layers_node_count[1]);

    float *output = malloc(layers_node_count[2] * sizeof(float));
    mat_multiply(output, weights_ho, hidden, layers_node_count[2],
                 layers_node_count[1], 1);
    mat_add(output, output, bias_o, layers_node_count[2], 1);
    mat_apply_softmax(output, layers_node_count[2]);

    mat_print(output, layers_node_count[2], 1);

    size_t max_index = 0;
    for (size_t i = 0; i < layers_node_count[2]; i++) {
        if (output[i] > output[max_index]) {
            max_index = i;
        }
    }
    printf("Neural Network guessed %zu with %.2f%% confidence\n", max_index,
           output[max_index] * 100);

    free(input);
    free(hidden);
    free(output);
    free(weights_ih);
    free(weights_ho);
    free(bias_h);
    free(bias_o);
}

void get_input(float *res, char *image_path) {
    surface = IMG_Load(image_path);
    if (!surface) {
        errx(1, "Could not load image");
    }
    int w, h;
    w = surface->w;
    h = surface->h;
    printf("Loaded image! %dx%d\n", w, h);

    Uint8 r, g, b;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            GetPixelColor(surface, x, y, &r, &g, &b);
            unsigned char average = (r + g + b) / 3;
            res[x * 28 + y] = average;
        }
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float a = res[x * 28 + y];
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
    }
}
