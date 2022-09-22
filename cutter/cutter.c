#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL_image.h>
#include <math.h>

#include "../utils.h"
#include "cutter.h"


SDL_Surface *surf;
int maxDist;

void DetectLines() {
    //Load the image
    surf = IMG_Load("DataSample/cutter/square.bmp");
    if (!surf) {
        errx(1, "Could not load image");
    }
    int w, h;
    w = surf->w;
    h = surf->h;
    printf("Image Loaded Succesfully with dimension: %dx%d!\n", w, h);


    
    // Creating the parameter space
    // x: 180 (-pi/2 to pi/2)  y: -max_dist to max_dist 
    // maxDist is the length of the diagonal of the image
    maxDist = (int) sqrt((double)w*w + h*h) + 1;

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

            GetPixelColor(surf, x, y, &r, &g, &b);
            unsigned char average = (r + g + b) / 3;
            if (average > 200){
                FillAcumulator(x, y, space);
            }
        }
    }
    
    for(int i = 0; i < maxDist * 360; i ++){
        if(i % 180 == 0)
            printf("\n");
        if(space[i] >= 100){
            printf("\033[1;31m");
            printf("%3u ", space[i]);
        }
        if(space[i] >= 1){
            printf("\033[0m");
            printf("%3u ", space[i]);
        }
        else{
            printf("\033[0m");
            printf("   ");
        }
            
    }
    printf("\n");
    
    int y = -maxDist;
    for(int i = 0; i < maxDist * 360; i ++){
        if (space[i] >= 150){
            DrawLine(surf, i%180-90, y);
        }

        if(i%180 == 0) y++;
    }
    SDL_SaveBMP(surf, "test.bmp");

    // To avoid memory leak
    free(space);
    printf("memory freed\n");
}




void FillAcumulator(int x, int y, unsigned char * space){
    for (int teta = -90; teta < 90; teta += 1){
        // conevrting teta in radian
        float angle = teta * 3.141592 / 180; 
        int rho = x*cos(angle) + y*sin(angle) + maxDist;
        space[rho*180 + teta + 90] += 1;
    }
}




void DrawLine(SDL_Surface * surf, int teta, int rho){
    float costeta = cos(teta);
    float sinteta = sin(teta);

    SDL_Rect r;
    r.w = 1;
    r.h = 1;
    for (int x = 0; x < surf->w; x++){
        int y = (rho - x*costeta) / sinteta;
        r.x = x;
        r.y = y;
        SDL_FillRect(surf, &r, SDL_MapRGB(surf->format, 255, 0, 0));
    }
    
}
