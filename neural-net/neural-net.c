#include "err.h"
#include <SDL2/SDL_image.h>

#include "../utils.h"
#include "neural-net.h"

SDL_Surface *surface;

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
