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
    surface = IMG_Load("DataSample/cutter/2pixels.bmp");
    if (!surface) {
        errx(1, "Could not load image");
    }
    int w, h;
    w = surface->w;
    h = surface->h;
    printf("Image Loaded Succesfully with dimension: %dx%d!\n", w, h);


    // Creating the parameter space
    // x: 180 (-pi/2 to pi/2)  y: -max_dist to max_dist 
    // maxDist is the length of the diagonal of the image
    maxDist = (int) sqrt((double)w*w + h*h) + 1;
    printf("maxDist: %i\n", maxDist);

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
            if (average > 200){
                printf("found one at: (%i, %i)!  Average: %u\n", x, y, average);
                FillAcumulator(x, y, space);
            }
        }
    }

    for(int i = 0; i < maxDist * 360; i ++){
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




void FillAcumulator(int a, int b, unsigned char * space){
    //printf("%i %i\n", x, y);

    int x = a+1;
    int y = b+1;
    for (int teta = -90; teta <= 90; teta += 1){
        float angle = teta * 3.141592 / 180;
//        printf("%f\n", angle);
        int rho = x*cos(angle) + y*sin(angle) + maxDist;
  //      printf("%i, %i, %i \n", teta, rho, rho*180 + teta + 90);

        if(teta == -90 || teta == -45 || teta == 0 || teta == 45 || teta == 90){
            printf("for teta = %i, rho = %i\n", teta, rho-maxDist);

        }
        space[rho*180 + teta + 90] += 1;
    }
}
