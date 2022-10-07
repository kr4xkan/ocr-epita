#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL_image.h>

#include "../utils.h"
#include "cutter.h"


#define pi 3.1415926535

int maxDist;
int MT = 180; //max theta

int minAverage = 240;
unsigned int minPeak;


void DetectLines() {
    SDL_Surface *surface = LoadImage("DataSample/cutter/og1inverted.png");
    if (!surface) 
        errx(1, "Could not load image");

    int w = surface->w;
    int h = surface->h;
    int * pixels = surface->pixels;
    printf("Image Loaded Succesfully with dimension: %dx%d!\n", w, h);


    minPeak = h < w ? h/1.5 : w/1.5;
    
    // Creating the parameter space
    // theta: 360   y: max_dist (length of diagonal) 
    maxDist = (int) sqrt((double)w*w + h*h) + 1;

    // Allocating a big chunk of memory during the compilation
    unsigned int * space = calloc(maxDist * MT, sizeof(unsigned int));
    if(space == NULL)
        errx(1, "Could not create space");





    // Looking all the pixel for the white ones (lines)
    Uint8 r, g, b;
    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++){

            GetPixelColor(surface, x, y, &r, &g, &b);
            if ((r + g + b) / 3 >= minAverage){
                for (int theta = 0; theta < MT; theta++){
                    int rho = x*cos(theta*pi/180) + y*sin(theta*pi/180);
                    if (rho  < maxDist && rho > 0){
                        space[abs(rho)*MT + theta] += 1;
                    }
                }
            }
        }
    }
    
    
    //PrintMat(space);

    Uint32 color = SDL_MapRGB(surface->format, 255, 0, 0);
    int rho = 0;
    for(int i = 0; i < maxDist * MT; i++){
        if (i%MT == 0) 
            rho += 1;

        if (space[i] >= minPeak){

            double theta = i%MT * pi / 180;
            double a = cos(theta);
            double b = sin(theta);
            int x0 = a*rho;
            int y0 = b*rho;
            int x1 = x0 + 2000*(-b);
            int y1 = y0 + 2000*a;
            int x2 = x0 - 2000*(-b);
            int y2 = y0 - 2000*a;
            DrawLine(pixels, w, h, x1, y1, x2, y2, color);
        }
    }


    IMG_SavePNG(surface, "test.png");

    // To avoid memory leak
    free(space);
    SDL_FreeSurface(surface);
}




//----------------------------------UTILS------------------------------
void DrawLine(int *pixels,
          long int w,
          long int h,
          long int x1,
          long int y1,
          long int x2,
          long int y2,
          Uint32 color)
{
    int i, dx, dy, maxmove;
    int d, dinc1, dinc2;
    int x, xinc1, xinc2;
    int y, yinc1, yinc2;

    dx = x1 > x2 ? x1 - x2 : x2 - x1;
    dy = y1 > y2 ? y1 - y2 : y2 - y1;

    if (dx >= dy) {
    maxmove = dx + 1;
    d = (2 * dy) - dx;
    dinc1 = 2 * dy;
    dinc2 = (dy - dx) * 2;
    xinc1 = 1;
    xinc2 = 1;
    yinc1 = 0;
    yinc2 = 1;
    } else {
    maxmove = dy + 1;
    d = (2 * dx) - dy;
    dinc1 = 2 * dx;
    dinc2 = (dx - dy) * 2;
    xinc1 = 0;
    xinc2 = 1;
    yinc1 = 1;
    yinc2 = 1;
    }

    if (x1 > x2) {
    xinc1 = -xinc1;
    xinc2 = -xinc2;
    }

    if (y1 > y2) {
    yinc1 = -yinc1;
    yinc2 = -yinc2;
    }

    x = x1;
    y = y1;

    for (i = 0; i < maxmove; ++i) {
    if (x >= 0 && x < w && y >= 0 && y < h) pixels[y * w + x] = color;
    if (d < 0) {
        d += dinc1;
        x += xinc1;
        y += yinc1;
    } else {
        d += dinc2;
        x += xinc2;
        y += yinc2;
    }
    }
}


void PrintMat(unsigned int * space){
    for(int i = 0; i < maxDist * MT; i ++){
        if(i % MT == 0)
            printf("\n");
        if(space[i] >= minPeak){ 
            printf("\033[1;31m");
            printf("%3u ", space[i]);
        }
        else if(space[i] >= 1){
            printf("\033[0m");
            printf("%3u ", space[i]);
        }
        else{
            printf("\033[0m");
            printf("   ");
        }
            
    }
    printf("\n");
}
