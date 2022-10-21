#include <SDL2/SDL_surface.h>
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

int accumulatorSize;
int maxDist;
unsigned int minPeak;

double cosArray[maxTheta];
double sinArray[maxTheta];


unsigned int* DetectLines(SDL_Surface *surface){
    /**
     * Detect the lines of the surface given as parameter
     * return a 2D array (the peaks of the array are the line in the parameter space)
     */

    int w = surface->w;
    int h = surface->h;
    printf("Image has %i width and %i height\n", w, h);

    // Creating the parameter accumulator
    // x: maxTheta    y: max_dist (length of diagonal) 
    maxDist = (int) sqrt((double)w*w + h*h) + 1;
    accumulatorSize = maxTheta * maxDist;
    unsigned int * accumulator = calloc(accumulatorSize, sizeof(unsigned int));
    if(accumulator == NULL)
        errx(1, "Could not create accumulator");


    // Compute all cos and sin values beforehand to avoid doing it 69420 times
    for (int i = 0; i < maxTheta; i++){
        cosArray[i] = cos(i*pi/180);
        sinArray[i] = sin(i*pi/180); 
    }


    FillAcumulator(surface, accumulator);
    minPeak = FindMinPeak(accumulator, accumulatorSize); 
    FilterLines(accumulator, accumulatorSize);

    return accumulator;
}


void FillAcumulator(SDL_Surface *surface, unsigned int *accumulator){
    /**
     * Check for every point in the image and draw it's sinus curve in the accumulator
     * Return the filled accumulator
     */

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
                        accumulator[rho*maxTheta + theta] += 1;
                    }
                }
            }
        }
    }
}


unsigned int FindMinPeak(unsigned int * accumulator, int accumulatorSize){
    /**
     * Compute the minimal value for a point to be consider a peak in the accumulator
     */
    unsigned int maxPeak = 0;
    for (int i = 0; i < accumulatorSize; i++){
        if (accumulator[i] > maxPeak)
            maxPeak = accumulator[i];
    }

    return maxPeak * ratio;
}


void FilterLines(unsigned int * accumulator, int accumulatorSize){
    /**
     * Remove lines that are too similar to another one
     */

    unsigned int rhoValues[100];
    unsigned int thetaValues[100];
    int maxGap = sqrt(maxDist);

    size_t len = 0;
    int rho = 0;
    for (int i = 0; i < accumulatorSize; i++){
        unsigned int val = accumulator[i];

        if (val >= minPeak && val >= accumulator[i-1] && val >= accumulator[i+1] && val >= accumulator[i-maxTheta] && val >= accumulator[i+maxTheta]){
            int theta = i%maxTheta;
            if (!AlreadyExist(theta, rho, rhoValues, thetaValues, len, maxGap)){
                rhoValues[len] = rho;
                thetaValues[len] = theta;
                len++;
            } 
            else{
                accumulator[i] = 0;
            }
        }
        else{
            accumulator[i] = 0;
        }

        if (i % maxTheta == 0)
            rho++;

    }
}



int AlreadyExist(int theta, int rho, unsigned int rhoValues[], unsigned int thetaValues[], size_t len, int maxGap){
    /**
     * Check if a line is similar to an already existing one
     * Return 1 if the line already exist, 0 else 
     */

    for (size_t i = 0; i < len; i++){
        int dTheta = abs((int) thetaValues[i] - theta);
        int dRho = abs((int) rhoValues[i] - rho);
        if ((dTheta <= 4 || dTheta >= 356 ) && dRho <= maxGap){
            return 1;
        }
    }
    return 0;
}






SDL_Surface* CheckRotation(SDL_Surface *surface, unsigned int *accumulator){
    /** 
     * Compute an average angle to rotate the image 
     * Return NULL if the surface does not need rotation
     *             the rotated surface else
     */

    int count = 0;
    int sum = 0;

    for(int i = 0; i < accumulatorSize; i++){

        unsigned int val = accumulator[i];
        if (val != 0){

            // computing an average angle to rotate
            int theta = i%maxTheta;
            if (theta < 90){
                sum += theta;
                count++;
            }
        }
    }


    SDL_Surface *rotated = NULL;
    int angle = sum / count;
    if (abs(90-angle) > 1){

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
    /**
     * Rotate a surface and return the new one
     */

    SDL_Surface* dest;
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
        errx(1, "Could not create accumulator");



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
                Uint32 color = GetPixelData(surface, bx, by);
                SetPixelData(dest, i, j, color);
            }
        }
    }

    return dest;
}





unsigned int* DetectIntersections(SDL_Surface *surface, unsigned int *accumulator){
    /**
     * Detect the intersections of every lines in the accumulator by drawing them in the normal space
     * Return the normal space 
     */

    int w = surface->w;
    int h = surface->h;

    // Creating the normalSpace
    unsigned int *normalSpace = calloc(w*h, sizeof(unsigned int));
    if(normalSpace == NULL)
        errx(1, "Could not create normalSpace");


    int rho = 0;
    for(int i = 0; i < accumulatorSize; i++){

        int theta = i%maxTheta;
        if (accumulator[i] != 0){

            //convert the point in the accumulator space in a line in the normal space
            // to do so: compute 2 point of the line and draw it
            double thetaRad = theta * pi / 180;
            double a = cos(thetaRad);
            double b = sin(thetaRad);
            int x0 = a*rho;
            int y0 = b*rho;
            int x1 = x0 + 2000*(-b);
            int y1 = y0 + 2000*a;
            int x2 = x0 - 2000*(-b);
            int y2 = y0 - 2000*a;

            ComputeLine(normalSpace, w, h, x1, y1, x2, y2);
        }

        if (theta%maxTheta == 0) 
            rho ++;
    }
    return normalSpace;
}


void ComputeLine(unsigned int *normalSpace, long int w, long int h, 
        long int x1, long int y1, long int x2, long int y2){
    /**
     * draw a line in the normal space
     */

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
    } 
    else {
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
        if (x >= 0 && x < w && y >= 0 && y < h) normalSpace[y * w + x] += 1;
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





void CropSquares(SDL_Surface *surface, unsigned int *accumulator){
    int w = surface->w;
    int h = surface->h;

    int x = 0, y = 0;
    for (long i = 0; i < w*h; i++){
        unsigned int val = accumulator[i];

        if (val >= 2){
            
        }

        x++; 
        if (x == h){
            y ++;
            x = 0;
        }
    }
}


//----------------------------UTILS--------------------------
void PrintMat(unsigned int * accumulator){
    for(int i = 0; i < accumulatorSize; i ++){
        if(i % maxTheta == 0)
            printf("\n");

        unsigned int val = accumulator[i];
        if(val >= minPeak){ 
            //       if (val >= accumulator[i-1] && val >= accumulator[i+1] && val >= accumulator[i-maxTheta] && val >= accumulator[i+maxTheta]){
            printf("\033[1;31m");
            printf("%3u ", accumulator[i]);
            //     }
        }
        else if(val >= 1){
            printf("\033[0m");
            printf("%3u ", accumulator[i]);
        }
        else{
            printf("\033[0m");
            printf("   ");
        }

    }
    printf("\n");
}



void DrawLines(SDL_Surface *surface, unsigned int *accumulator, int *pixels){
    Uint32 color = SDL_MapRGB(surface->format, 255, 0, 0);

    int rho = 0;
    for(int i = 0; i < accumulatorSize; i++){

        int theta = i%maxTheta;

        unsigned int val = accumulator[i];
        // A peak has its value greater than minPeak and gretaer or equal than its 4 closest neighbours
        // first compare to the peak then left / right / top / bottom
        //if (val >= minPeak && val >= accumulator[i-1] && val >= accumulator[i+1] && val >= accumulator[i-maxTheta] && val >= accumulator[i+maxTheta])
        if (val >= minPeak){

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



void DrawIntersections(SDL_Surface *surface, unsigned int *accumulator){
    int w = surface->w;
    int h = surface->h;


    Uint32 color = SDL_MapRGB(surface->format, 255, 0, 0);
    int x = 0, y = 0;
    for (long i = 0; i < w*h; i++){
        unsigned int val = accumulator[i];

        if (val >= 2){
            for (int i = -3; i <= 3; i++){
                for (int j = -3; j <= 3; j++){
                    SetPixelData(surface, x+i, y+j, color);
                }

            }
        }

        x++; 
        if (x == h){
            y ++;
            x = 0;
        }
    }

}


