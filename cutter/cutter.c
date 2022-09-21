#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL_image.h>

#include "../utils.h"
#include "cutter.h"

SDL_Surface *surface;

void detect_lines() {
    surface = IMG_Load("DataSample/cutter/square.bmp");
    if (!surface) {
        errx(1, "Could not load image");
    }
    int w, h;
    w = surface->w;
    h = surface->h;
    printf("Loaded image! %dx%d\n", w, h);


    // Allocating a big chunk of memory during the compilation
    unsigned char * tableau = calloc(w*h, sizeof(unsigned char));
    if(tableau == NULL){
        errx(1, "Could not tableau");
    }

    Uint8 r, g, b;
    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++){

            GetPixelColor(surface, x, y, &r, &g, &b);
            unsigned char average = (r + g + b) / 3;
        }
    }
    // To avoid memory leak
    free(tableau);
}
