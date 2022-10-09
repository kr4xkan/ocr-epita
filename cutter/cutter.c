#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL_image.h>

#include "../utils.h"
#include "cutter.h"


#define pi 3.1415926535

int maxTheta;

int minAverage = 50;
unsigned int minPeak;
float ratio = 0.50;


void DetectLines() {
    //Load the surface
    //SDL_Surface *surface = LoadImage("DataSample/cutter/og1.png");
    SDL_Surface *surface = LoadImage("DataSample/original/image_04.jpeg");
    if (!surface) 
        errx(1, "Could not load image");

    int w = surface->w;
    int h = surface->h;
    int * pixels = surface->pixels;
    printf("Image Loaded Succesfully with dimension: %dx%d!\n", w, h);

    minPeak = w < h ? w*ratio : h*ratio;


    // Creating the parameter space
    // x: maxTheta    y: max_dist (length of diagonal) 
    maxTheta = 360;
    int maxDist = (int) sqrt((double)w*w + h*h) + 1;
    int spaceSize = maxTheta * maxDist;
    unsigned int * space = calloc(spaceSize, sizeof(unsigned int));
    if(space == NULL)
        errx(1, "Could not create space");



    // Compute all cos and sin values beforehand to avoid doing it 69420 times
    double cosArray[maxTheta];
    double sinArray[maxTheta];
    for (int i = 0; i < maxTheta; i++){
        cosArray[i] = cos(i*pi/180);
        sinArray[i] = sin(i*pi/180);

    }


    // Looking all the pixel for the white ones (lines)
    Uint8 r, g, b;
    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++){

            GetPixelColor(surface, x, y, &r, &g, &b);
            if ((r + g + b) / 3 <= minAverage){
                SDL_Rect r;
                r.w = 1;
                r.h = 1;
                r.x = x;
                r.y = y;
                //SDL_FillRect(surface, &r, SDL_MapRGB(surface->format, 0, 0, 255));


                //compute all the values of rho and theta for the given point
                for (int theta = 0; theta < maxTheta; theta++){
                    int rho = x*cosArray[theta] + y*sinArray[theta];
                    if (rho > 0 && rho < maxDist){
                        space[rho*maxTheta + theta] += 1;
                    }
                }
            }
        }
    }
    DetectPeaks(space, spaceSize); 
    
    PrintMat(space, spaceSize);



    //Drawing the lines
    Uint32 color = SDL_MapRGB(surface->format, 255, 0, 0);
    int rho = 0;
    for(int i = 0; i < spaceSize; i++){

        unsigned int val = space[i];
        if (val >= minPeak){
            //       left                right                top                   bottom
            if (val >= space[i-1] && val >= space[i+1] && val >= space[i-maxTheta] && val >= space[i+maxTheta]){
                double theta = i%maxTheta * pi / 180;
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

        if (i%maxTheta == 0) 
            rho ++;
    }


    IMG_SavePNG(surface, "test.png");

    // To avoid memory leak
    free(space);
    SDL_FreeSurface(surface);
}




void DetectPeaks(unsigned int * space, int spaceSize){
    unsigned int maxPeak = 0;
    for (int i = 0; i < spaceSize; i++){
        if (space[i] > maxPeak)
            maxPeak = space[i];
    }

    minPeak = maxPeak*ratio;
}



//----------------------------------UTILS--------------------------
void PrintMat(unsigned int * space, int spaceSize){
    for(int i = 0; i < spaceSize; i ++){
        if(i % maxTheta == 0)
            printf("\n");
        if(space[i] >= minPeak){ 
            unsigned int val = space[i];
            if (val >= space[i-1] && val >= space[i+1] && val >= space[i-maxTheta] && val >= space[i+maxTheta]){
                printf("\033[1;31m");
                printf("%3u ", space[i]);
            }
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
