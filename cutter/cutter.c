#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL_image.h>

#include "../utils.h"
#include "cutter.h"


#define pi 3.1415926535
#define maxTheta 360

//min value needed by a pixel to be counted as detected
int minAverage = 200;

//minPeak = maxPeak (biggest line) * ratio 
float ratio = 0.50;

int spaceSize;
int maxDist;
unsigned int minPeak;

double cosArray[maxTheta];
double sinArray[maxTheta];


unsigned int* DetectLines(SDL_Surface *surface){

    int w = surface->w;
    int h = surface->h;
    printf("Image Loaded Succesfully with dimension: %dx%d!\n", w, h);



    // Creating the parameter space
    // x: maxTheta    y: max_dist (length of diagonal) 
    maxDist = (int) sqrt((double)w*w + h*h) + 1;
    spaceSize = maxTheta * maxDist;
    unsigned int * space = calloc(spaceSize, sizeof(unsigned int));
    if(space == NULL)
        errx(1, "Could not create space");



    // Compute all cos and sin values beforehand to avoid doing it 69420 times
    for (int i = 0; i < maxTheta; i++){
        cosArray[i] = cos(i*pi/180);
        sinArray[i] = sin(i*pi/180);
    }


    FillAcumulator(surface, space);
    minPeak = FindMinPeak(space, spaceSize); 

    return space;
}






unsigned int FindMinPeak(unsigned int * space, int spaceSize){
    unsigned int maxPeak = 0;
    for (int i = 0; i < spaceSize; i++){
        if (space[i] > maxPeak)
            maxPeak = space[i];
    }

    return maxPeak * ratio;
}


void FillAcumulator(SDL_Surface *surface, unsigned int *space){

    Uint8 r, g, b;
    //Cycle trhough all the pixels to find the white ones
    for (int y = 0; y < surface->h; y++){
        for (int x = 0; x < surface->w; x++){

            GetPixelColor(surface, x, y, &r, &g, &b);
            if ((r + g + b) / 3 >= minAverage){

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
}



SDL_Surface* CheckRotation(SDL_Surface *surface, unsigned int *space){

    // Find the angle to rotate the image (if needed) 
    int div = 0;
    int sum = 0;
    int rotateNeeded = 1;

    int rho = 0;
    for(int i = 0; i < spaceSize; i++){

        unsigned int val = space[i];
        // A peak has its value greater than minPeak and gretaer or equal than its 4 closest neighbours
        // neighbours:                 left                right                      top                      bottom
        if (val >= minPeak && val >= space[i-1] && val >= space[i+1] && val >= space[i-maxTheta] && val >= space[i+maxTheta]){

            // computing an average angle to rotate
            int theta = i%maxTheta;
            if (theta == 90)
                rotateNeeded = 0;
            else if (theta < 90){
                sum += theta;
                div++;
            }
        }

        if (i%maxTheta == 0) 
            rho ++;
    }



    SDL_Surface *rotated = NULL;
    if (rotateNeeded){
        int angle = sum / div;

        if (angle >= 45){
            printf("Rotate %i° Clockwise\n", 90-angle);
            rotated = RotateSurface(surface, -90+angle);
        }
        else {
            printf("Rotate %i° CounterClockwise\n", angle);
            rotated = RotateSurface(surface, angle);
        }

    }

    return rotated;
}




SDL_Surface* RotateSurface(SDL_Surface* surface, float angle){
    SDL_Surface* dest;
    Uint32 color;
    int mx, my, mxdest, mydest;
    int bx, by;

    // Switch angle to radian
    angle = -angle * pi / 180.0;

    // Cache values 
    float cosVal = cos(angle);
    float sinVal = sin(angle);

    // Size of new surface  
    double widthDest = ceil(surface->w * fabs(cosVal) + surface->h * fabs(sinVal));
    double heightDest = ceil(surface->w * fabs(sinVal) + surface->h * fabs(cosVal));


    //Create surface
    dest = SDL_CreateRGBSurface(SDL_SWSURFACE, widthDest, heightDest, 
            surface->format->BitsPerPixel,
            surface->format->Rmask, 
            surface->format->Gmask, 
            surface->format->Bmask, 
            surface->format->Amask);

    if(dest==NULL)
        errx(1, "Could not create space");



    mxdest = dest->w / 2;
    mydest = dest->h / 2;
    mx = surface->w / 2;
    my = surface->h / 2;

    for(int j = 0; j < dest->h; j++){
        for(int i = 0; i < dest->w; i++)
        {

            bx = (ceil (cosVal * (i-mxdest) + sinVal * (j-mydest) + mx));
            by = (ceil (-sinVal * (i-mxdest) + cosVal * (j-mydest) + my));

            //Check if the found coordinates are inside the new surface
            if (bx >=0 && bx < surface->w && by >= 0 && by < surface->h){
                color = GetPixelData(surface, bx, by);
                SetPixelData(dest, i, j, color);
            }
        }
    }

    return dest;
}






//----------------------------------UTILS--------------------------
void PrintMat(unsigned int * space){
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



void DrawLines(SDL_Surface *surface, unsigned int *space, int *pixels){
    Uint32 color = SDL_MapRGB(surface->format, 255, 0, 0);

    int rho = 0;
    for(int i = 0; i < spaceSize; i++){

        int theta = i%maxTheta;

        unsigned int val = space[i];
        // A peak has its value greater than minPeak and gretaer or equal than its 4 closest neighbours
        // neighbours:                 left                right                      top                      bottom
        if (val >= minPeak && val >= space[i-1] && val >= space[i+1] && val >= space[i-maxTheta] && val >= space[i+maxTheta]){


            //Drawing the corresponding lines
            double thetaRad = theta * pi / 180;
            double a = cos(thetaRad);
            double b = sin(thetaRad);
            int x0 = a*rho;
            int y0 = b*rho;
            int x1 = x0 + 2000*(-b);
            int y1 = y0 + 2000*a;
            int x2 = x0 - 2000*(-b);
            int y2 = y0 - 2000*a;
             
            DrawLine(pixels, surface->w, surface->h, x1, y1, x2, y2, color);

        }

        if (i%maxTheta == 0) 
            rho ++;
    }
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
