#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL_image.h>
#include <math.h>

#include "../utils.h"
#include "cutter.h"

int maxDist;

void DetectLines() {
    //Load the image
    SDL_Surface *surface;
    surface = IMG_Load("DataSample/cutter/square.bmp");
    if (!surface) {
        errx(1, "Could not load image");
    }
    int w, h;
    w = surface->w;
    h = surface->h;
    printf("Image Loaded Succesfully with dimension: %dx%d!\n", w, h);


    // define the parameter spaces 
    maxDist = (int) sqrt((double)w*w + h*h) + 1;

    // Allocating a big chunk of memory during the compilation
    unsigned char * space = calloc(maxDist * 180, sizeof(unsigned char));
    if(space == NULL){
        errx(1, "Could not create space");
    }
    printf("Parameter space created succesfully\n");


    Uint8 r, g, b;
    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++){

            GetPixelColor(surface, x, y, &r, &g, &b);
            unsigned char average = (r + g + b) / 3;
            if (average > 0){
                FillAcumulator(x, y, space);
            }
        }
    }
    // To avoid memory leak
    free(space);
}




void FillAcumulator(int x, int y, unsigned char * space){
    printf("%i, %i\n", x, y);
    printf("-------------------------------------------------------\n");
    for (int teta = -90; teta <= 90; teta += 3){
        int rho = x*cos(teta) + y*sin(teta);
        printf("%i, %i, %i \n", teta, rho, rho*maxDist+teta);
        space[rho*maxDist + teta] += 1;
    }
}
