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
    surface = IMG_Load("DataSample/cutter/minisquare.bmp");
    if (!surface) {
        errx(1, "Could not load image");
    }
    int w, h;
    w = surface->w;
    h = surface->h;
    printf("Image Loaded Succesfully with dimension: %dx%d!\n", w, h);


    // define the parameter spaces 
    maxDist = (int) sqrt((double)w*w + h*h) + 1;
    printf("%i\n", maxDist * 180);

    // Allocating a big chunk of memory during the compilation
    unsigned char * space = calloc(maxDist * 360, sizeof(unsigned char));
    if(space == NULL){
        errx(1, "Could not create space");
    }
    printf("Parameter space created succesfully!\n");

    // Looking all the pixel for the white ones (lines)
    Uint8 r, g, b;
    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++){

            GetPixelColor(surface, x, y, &r, &g, &b);
            unsigned char average = (r + g + b) / 3;
            if (average > 0){
                FillAcumulator(x, y, space);
            }
        }
        printf("%i  ", y);
    }
    
    for(int i = 0; i < maxDist * 180; i ++){
        if(i % 180 == 0)
            printf("\n");
        if(space[i] != 0)
            printf("\033[1;31m");
        else
            printf("\033[0m");
            
        printf("%3u ", space[i]);
    }

    // To avoid memory leak
    free(space);
    free(surface);
    printf("memory freed\n");
}




void FillAcumulator(int x, int y, unsigned char * space){
    //printf("%i %i\n", x, y);
    for (int teta = -90; teta <= 90; teta += 1){
        float angle = teta * 3.141592 / 180;
//        printf("%f\n", angle);
        int rho = x*cos(angle) + y*sin(angle) + maxDist;
  //      printf("%i, %i, %i \n", teta, rho, rho*180 + teta + 90);
        space[rho*180 + teta + 90] += 1;
    }
}
