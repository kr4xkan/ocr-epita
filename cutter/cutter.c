#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL_image.h>

#include "../utils.h"
#include "cutter.h"


#define pi 3.1415926535


int maxTheta;

//min value needed by a pixel to be counted as detected
int minAverage = 200;


//minPeak = maxPeak (biggest line) * ratio 
float ratio = 0.50;


void DetectLines() {
    //Load the surface
    //SDL_Surface *surface = LoadImage("DataSample/cutter/og1.png");
    SDL_Surface *surface = LoadImage("DataSample/cutter/og1rotate.png");
    if (!surface) 
        errx(1, "Could not load image");

    int w = surface->w;
    int h = surface->h;
    int * pixels = surface->pixels;
    printf("Image Loaded Succesfully with dimension: %dx%d!\n", w, h);



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




    unsigned int minPeak = FindMinPeak(space, spaceSize); 

    //PrintMat(space, spaceSize, minPeak);

    int div = 0;
    int sum = 0;
    int rotateNeeded = 1;

    Uint32 color = SDL_MapRGB(surface->format, 255, 0, 0);

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
             
             
            DrawLine(pixels, w, h, x1, y1, x2, y2, color);

        }

        if (i%maxTheta == 0) 
            rho ++;
    }


    if (rotateNeeded){
        int angle = sum / div;

        SDL_Surface *rotated;
        if (angle >= 45){
            printf("Rotate %i° Clockwise\n", 90-angle);
            rotated = RotateSurface(surface, -90+angle);
        }
        else{
            printf("Rotate %i° CounterClockwise\n", angle);
            rotated = RotateSurface(surface, angle);
        }


        IMG_SavePNG(rotated, "rotate.png");
        SDL_FreeSurface(rotated);
    }



    IMG_SavePNG(surface, "test.png");

    // To avoid memory leak
    free(space);
    SDL_FreeSurface(surface);
}




unsigned int FindMinPeak(unsigned int * space, int spaceSize){
    unsigned int maxPeak = 0;
    for (int i = 0; i < spaceSize; i++){
        if (space[i] > maxPeak)
            maxPeak = space[i];
    }

    return maxPeak * ratio;
}




SDL_Surface* RotateSurface(SDL_Surface* surface, float angle){
    SDL_Surface* dest;
    Uint32 couleur;
    int mx, my, mxdest, mydest;
    int bx, by;

    //switch angle to radian
    angle = -angle * pi / 180.0;

    /*pour éviter pleins d'appel, on stocke les valeurs*/
    float cosVal = cos(angle);
    float sinVal = sin(angle);

    /*calcul de la taille de l'image de dest*/
    double largeurdest = ceil(surface->w * fabs(cosVal) + surface->h * fabs(sinVal));
    double hauteurdest = ceil(surface->w * fabs(sinVal) + surface->h * fabs(cosVal));


    //Create surface
    dest = SDL_CreateRGBSurface(SDL_SWSURFACE, largeurdest, hauteurdest, 
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
            /* on détermine la valeur de pixel qui correspond le mieux pour la position
             * i,j de la surface de dest */

            /* on détermine la meilleure position sur la surface d'surface en appliquant
             * une matrice de rotation inverse
             */

            bx = (ceil (cosVal * (i-mxdest) + sinVal * (j-mydest) + mx));
            by = (ceil (-sinVal * (i-mxdest) + cosVal * (j-mydest) + my));

            //Check if the found coordinates are inside the new surface
            if (bx >=0 && bx < surface->w && by >= 0 && by < surface->h){
                couleur = GetPixelData(surface, bx, by);
                SetPixelData(dest, i, j, couleur);
            }
        }
    }

    return dest;
}

//----------------------------------UTILS--------------------------
void PrintMat(unsigned int * space, int spaceSize, unsigned int minPeak){
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
