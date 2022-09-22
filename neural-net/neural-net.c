#include "err.h"
#include <SDL2/SDL_image.h>
#include <stdlib.h>

#include "../utils.h"
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

    // Build neural network
    float *weights_ih =
        malloc(layers_node_count[0] * layers_node_count[1] * sizeof(float));
    
    float *weights_ho =
        malloc(layers_node_count[1] * layers_node_count[2] * sizeof(float));

    free(weights_ih);
    free(weights_ho);
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
