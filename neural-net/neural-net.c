#include "err.h"
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <time.h>

#include "../utils.h"
#include "matrix.h"
#include "neural-net.h"

SDL_Surface *surface;

int main(int argc, char **argv) {
    if (argc < 4 || argc > 8) {
        errx(1, "\nUsage:\n"
                "./neural nb_input nb_hidden1 ... nb_hidden(n) nb_output\n\n"
                "Maximum of 5 hidden layers");
    }

    int layer_count = argc - 1;
    int layers_node_count[6];

    char *ptr;
    for (int i = 1; i < argc; i++) {
        layers_node_count[i - 1] = strtol(argv[i], &ptr, 10);
    }

    // Initialize randomizer
    srand((unsigned int)time(NULL));

    // Build neural network
    float *weights_ih =
        malloc(layers_node_count[0] * layers_node_count[1] * sizeof(float));
    size_t len_w_ih = layers_node_count[0] * layers_node_count[1];

    float *weights_ho =
        malloc(layers_node_count[1] * layers_node_count[2] * sizeof(float));
    size_t len_w_ho = layers_node_count[1] * layers_node_count[2];

    mat_randomize(weights_ih, len_w_ih);
    mat_randomize(weights_ho, len_w_ho);

    mat_print(weights_ih, layers_node_count[0], layers_node_count[1]);

    float *bias_h = malloc(layers_node_count[1] * sizeof(float));
    float *bias_o = malloc(layers_node_count[2] * sizeof(float));

    float t1[] = {1,2,3,4,5,6};
    float t2[] = {6,6,6,6,6,6};
    float tr[9];
    mat_multiply(tr, t1, t2, 3, 2, 3);

    mat_print(tr, 3, 3);

    free(weights_ih);
    free(weights_ho);
    free(bias_h);
    free(bias_o);
}

void neural_test() {
    surface = IMG_Load("DataSample/neuralnet/image.bmp");
    if (!surface) {
        errx(1, "Could not load image");
    }
    int w, h;
    w = surface->w;
    h = surface->h;
    printf("Loaded image! %dx%d\n", w, h);

    unsigned char pixels[28][28];

    Uint8 r, g, b;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            GetPixelColor(surface, x, y, &r, &g, &b);
            unsigned char average = (r + g + b) / 3;
            pixels[x][y] = average;
        }
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            printf("%c", pixels[x][y] > 200 ? ' ' : '#');
        }
        printf("\n");
    }
}
