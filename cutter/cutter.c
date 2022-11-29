#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <stdlib.h>

#include "../utils.h"
#include "cutter.h"
#include "crop-manager.h"

#include <string.h>
#include <time.h>

#define pi 3.1415926535
#define maxTheta 360

// min value needed by a pixel to be counted as detected
int minAverage = 200;

// minPeak = maxPeak (biggest line) * ratio
float ratio = 0.33;

size_t accumulatorSize;
size_t maxDist;
unsigned int minPeak;


double cosArray[maxTheta];
double sinArray[maxTheta];


int main(int argc, char **argv) {
    printf("\n");
    if (argc < 2) {
        errx(1, "Usage: ./cutter --cut|--rotate");
    }


    if (strcmp(argv[1], "--cut") == 0) {
        if (argc != 3) {
            errx(1, "\nUsage: ./cutter --cut <image_path>\nExample:" 
                    "./cutter --cut original/image_01.jpeg");
        }
        return MainCutter(argv[2]);

    } 

    else if (strcmp(argv[1], "--rotate") == 0){
        if (argc != 4){
            errx(1, "\nUsage: ./rotate --rotate <image_path> <angle>\nExample: ./rotate "
                    "image.png 20");
        }

        SDL_Surface *surface = LoadImage(argv[2]);
        if (!surface)
            errx(1, "Could not load image");

        float angle = atof(argv[3]);

        SDL_Surface *rotated = RotateSurface(surface, angle);

        IMG_SavePNG(rotated, "rotated.png");

        return 0;
    }
    else{
        errx(1, "Usage: ./cutter --cut|--rotate");
    }
    
}
    
int MainCutter(char *path) {
    clock_t t = clock();

    // WARNING: NEVER USE TWO ACCUMULATOR AT THE SAME TIME
    // SOME VALUES ARE COMPUTED WITH CreateAccumulator AND WILL CHANGE IF USE WITH
    // ANOTHER ACCUMULATOR

    // Load the surface
    SDL_Surface *surface;
    if (strcmp(path, "0") == 0){
        printf("Using default image\n");
        surface = LoadImage("../DataSample/cutter/og1rotated.png");
    }
    else{
        surface = LoadImage(path);
    }
    if (!surface)
        errx(1, "Could not load image");


    unsigned int *accumulator = CreateAccumulator(surface);
    Line *lines = DetectLines(accumulator, accumulatorSize);

    SDL_Surface *surfaceRotated = CheckRotation(surface, accumulator);

    if (surfaceRotated == NULL) {
        FilterLines(accumulator, accumulatorSize, lines);
        unsigned int *space = DetectIntersections(surface, accumulator);
        //CropSquares(surface, space);

        DrawLines(surface, accumulator, surface->pixels);
        DrawIntersections(surface, space);
        IMG_SavePNG(surface, "result.png");

        free(space);
    }
    else{
        unsigned int *accumulatorRotated = CreateAccumulator(surfaceRotated);
        Line *lines = DetectLines(accumulatorRotated, accumulatorSize);
        FilterLines(accumulatorRotated, accumulatorSize, lines);

        unsigned int *spaceRotated = DetectIntersections(surfaceRotated, accumulatorRotated);
        //CropSquares(surfaceRotated, spaceRotated);

        DrawLines(surfaceRotated, accumulatorRotated, surfaceRotated->pixels);
        DrawIntersections(surfaceRotated, spaceRotated);
        IMG_SavePNG(surfaceRotated, "result.png");

        free(spaceRotated);
        free(accumulatorRotated);
        SDL_FreeSurface(surfaceRotated);
    } 

    free(accumulator);
    SDL_FreeSurface(surface);

    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds

    printf("\n%f seconds to execute \n", time_taken);
    return 0;
}


unsigned int *CreateAccumulator(SDL_Surface *surface) {
    /**
     * Detect the lines of the surface given as parameter
     * return a 2D array (the peaks of the array are the line in the parameter
     * space)
     */

    int w = surface->w;
    int h = surface->h;
    printf("Image has %i width and %i height\n", w, h);

    // Creating the parameter accumulator
    // x: maxTheta    y: max_dist (length of diagonal)
    maxDist = (size_t)sqrt((double)w * w + h * h) + 1;
    accumulatorSize = maxTheta * maxDist;
    unsigned int *accumulator = calloc(accumulatorSize, sizeof(unsigned int));
    if (accumulator == NULL)
        errx(1, "Could not create accumulator");

    // Compute all cos and sin values beforehand to avoid doing it 69420 times
    for (size_t i = 0; i < maxTheta; i++) {
        cosArray[i] = cos(i * pi / 180);
        sinArray[i] = sin(i * pi / 180);
    }

    FillAcumulator(surface, accumulator);
    minPeak = FindMinPeak(accumulator, accumulatorSize);

    return accumulator;
}

void FillAcumulator(SDL_Surface *surface, unsigned int *accumulator) {
    /**
     * Check for every point in the image and draw it's sinus curve in the
     * accumulator Return the filled accumulator
     */

    Uint8 r, g, b;
    // Cycle trhough all the pixels to find the white ones
    for (size_t y = 0; y < (size_t)surface->h; y++) {
        for (size_t x = 0; x < (size_t)surface->w; x++) {

            GetPixelColor(surface, x, y, &r, &g, &b);
            if ((r + g + b) / 3 >= minAverage) {
                
                // compute all the values of rho and theta for the given point
                for (size_t theta = 0; theta < maxTheta; theta++) {
                    int rho = x * cosArray[theta] + y * sinArray[theta];
                    if (rho >= 0 && rho < (int)maxDist) {
                        accumulator[rho * maxTheta + theta]++;
                    }
                }
            }
        }
    }
}

unsigned int FindMinPeak(unsigned int *accumulator, size_t accumulatorSize) {
    /**
     * Compute the minimal value for a point to be consider a peak in the
     * accumulator
     */
    unsigned int maxPeak = 0;
    for (size_t i = 0; i < accumulatorSize; i++) {
        if (accumulator[i] > maxPeak)
            maxPeak = accumulator[i];
    }
    return maxPeak * ratio;
}

Line* DetectLines(unsigned int *accumulator, size_t accumulatorSize) {
    /**
     * Remove lines that are too similar to another one
     */

    Line *lines = malloc(500*sizeof(Line));
    size_t len = 0;

    size_t maxGap = sqrt(maxDist);

    //Removing duplicates 
    int rho = 0;
    int theta = 0;
    for (size_t i = 0; i < accumulatorSize; i++) {

        unsigned int val = accumulator[i];
        if (CheckPeak(accumulator, accumulatorSize, i, val)) {
            Line line = {theta, rho, val, i};
            
            if (!AlreadyExist(lines, line, len, maxGap, accumulator)) {
                lines[len] = line;
                len++;
            }
        } 
        else {
            accumulator[i] = 0;
        }

        theta++;
        if (theta == maxTheta) {
            theta = 0;
            rho++;
        }

    }
    lines[len].accuPos = accumulatorSize+1;;
    return lines;
}

int AlreadyExist(Line *lines, Line line, size_t len, int maxGap,
        unsigned int *accumulator) {
    /**
     * Check if a line is similar to an already existing one
     * Return 1 if the line already exist, 2 if a bigger one is found, 0 else
     */

    for (size_t i = 0; i < len; i++) {
        Line iLine = lines[i];

        int dTheta = abs((int)(iLine.theta%180) - (int)(line.theta%180));
        int dRho = abs((int)(iLine.rho) - (int)(line.rho));

        if ((dTheta <= 10 || dTheta >= 170) && dRho <= maxGap) {
            // if two lines are similar keep the biggest one
            if (line.value > iLine.value){
                accumulator[iLine.accuPos] = 0;
                lines[i] = line;
            }
            else{
                accumulator[line.accuPos] = 0;
            }
            return 1;
        }
    }
    return 0;
}

int CheckPeak(unsigned int *accumulator, size_t accumulatorSize, size_t i,
              unsigned int val) {
    if (val < minPeak)
        return 0;
    // Left neighbour
    if (i % maxTheta != 0 && accumulator[i - 1] > val)
        return 0;
    // Right neighbour
    if (i % maxTheta != maxTheta - 1 && accumulator[i + 1] > val)
        return 0;
    // Top neighbour
    if (i >= maxTheta && accumulator[i - maxTheta] > val)
        return 0;
    // Bottom neighbour
    if (i < accumulatorSize - maxTheta && accumulator[i + maxTheta] > val)
        return 0;
    return 1;
}


void FilterLines(unsigned int *accumulator, size_t accumulatorSize,
                                                    Line* lines){

    size_t len = 0;
    while (lines[len].accuPos != accumulatorSize+1)
        len++;
    

    unsigned int *histo = calloc(3000, sizeof(unsigned int));

    size_t vert = 0, hori = 0;
    unsigned int rho1 = -1, rho2 = -1;

    for (size_t i = 0; i < len; i++){
        Line line = lines[i];

        if (line.theta % 90 >= 2 && line.theta % 90 <= 88){
            accumulator[line.accuPos] = 0;            
            lines[i].value = 0;
        }
        else{
            if(line.theta < 2){
                vert++;
                if (rho1 == (unsigned int)-1)
                    rho1 = line.rho;
                else{
                    histo[line.rho - rho1] += 1;
                    rho1 = line.rho;
                }
            }
            else{
                hori++;
                if (rho2 == (unsigned int)-1)
                    rho2 = line.rho;
                else{
                    histo[line.rho - rho2] += 1;
                    rho2 = line.rho;
                }
            }
        }
    }



    size_t range = 20;
    while (range > 0 && (hori != 10 || vert != 10)){
        size_t gap = FindGap(histo, range);

        char checkVert = -1, checkHori = -1;
        unsigned int rhoVert = -1, rhoHor = -1;

        for (size_t i = 0; i < len; i++){
            Line line = lines[i];

            if (line.value != 0 && line.theta < 2){
                if (checkVert == -1){
                    rhoVert = line.rho;
                    checkVert = 0;
                }
                else{
                    size_t dr = line.rho - rhoVert;
                    rhoVert = line.rho;
                }
            }
            else if (line.value != 0){
                if (checkHori == -1){
                    rhoHor = line.rho;
                    checkHori = 0;
                }
                else{
                    size_t dr = (line.rho - rhoHor) % gap;
                    if (dr > 10 && dr < gap-10){
                        if (!checkHori){
                            line.value = 0;
                            accumulator[line.accuPos] = 0;
                        }
                        checkHori = 0;
                    }
                    else {
                        checkHori = 1;
                    }
                    rhoHor = line.rho;
                }
            }
        }
        range--;
    }

    free(histo);
}



size_t FindGap(unsigned int *histo, size_t range){
    size_t current = 0;
    for (size_t i = 0; i <= range*2; i++)
        current += histo[i];

    size_t gap = range;
    size_t max = current;
    for (size_t i = range+2; i < 3000-range; i++){
        current -= histo[i-range-1];
        current += histo[i+range];
        if (current > max){
            gap = i;
            max = current;
        }
    }
    return gap;
    
}



SDL_Surface *CheckRotation(SDL_Surface *surface, unsigned int *accumulator) {
    /**
     * Compute an average angle to rotate the image
     * Return NULL if the surface does not need rotation
     *             the rotated surface else
     */

    int count = 0;
    int sum = 0;
    for (size_t i = 0; i < accumulatorSize; i++) {

        unsigned int val = accumulator[i];
        if (val != 0) {
            // computing an average angle to rotate
            size_t theta = i % maxTheta;
            if (theta == 90)
                return NULL;
            if (theta < 90) {
                sum += theta;
                count++;
            }
        }
    }

    if (count == 0)
        return NULL;

    SDL_Surface *rotated = NULL;
    int angle = sum / count;
    if (abs(angle) > 1) {
        if (angle >= 45) {
            printf("Rotate %i° Clockwise\n", 90 - angle);
            rotated = RotateSurface(surface, -90 + angle);
        } else {
            printf("Rotate %i° CounterClockwise\n", angle);
            rotated = RotateSurface(surface, angle);
        }
    }

    return rotated;
}

SDL_Surface *RotateSurface(SDL_Surface *surface, float angle) {
    /**
     * Rotate a surface and return the new one
     */

    SDL_Surface *dest;
    int mx, my, mxdest, mydest;
    int bx, by;

    // Switch angle to radian
    angle = -angle * pi / 180.0;

    // Cache values
    float cosVal = cos(angle);
    float sinVal = sin(angle);

    // Size of new surface
    double widthDest =
        ceil(surface->w * fabs(cosVal) + surface->h * fabs(sinVal));
    double heightDest =
        ceil(surface->w * fabs(sinVal) + surface->h * fabs(cosVal));

    // Create surface
    dest = SDL_CreateRGBSurface(SDL_SWSURFACE, widthDest, heightDest,
                                surface->format->BitsPerPixel,
                                surface->format->Rmask, surface->format->Gmask,
                                surface->format->Bmask, surface->format->Amask);

    if (dest == NULL)
        errx(1, "Could not create accumulator");

    mxdest = dest->w / 2;
    mydest = dest->h / 2;
    mx = surface->w / 2;
    my = surface->h / 2;

    for (int j = 0; j < dest->h; j++) {
        for (int i = 0; i < dest->w; i++) {

            bx = (ceil(cosVal * (i - mxdest) + sinVal * (j - mydest) + mx));
            by = (ceil(-sinVal * (i - mxdest) + cosVal * (j - mydest) + my));

            // Check if the found coordinates are inside the new surface
            if (bx >= 0 && bx < surface->w && by >= 0 && by < surface->h) {
                Uint32 color = GetPixelData(surface, bx, by);
                SetPixelData(dest, i, j, color);
            }
        }
    }

    return dest;
}




unsigned int *DetectIntersections(SDL_Surface *surface,
                                  unsigned int *accumulator) {
    /**
     * Detect the intersections of every lines in the accumulator by drawing
     * them in the normal space Return the normal space
     */

    int w = surface->w;
    int h = surface->h;

    // Creating the normalSpace
    unsigned int *normalSpace = calloc(w * h, sizeof(unsigned int));
    if (normalSpace == NULL)
        errx(1, "Could not create normalSpace");

    int rho = 0;
    int theta = 0;
    for (size_t i = 0; i < accumulatorSize; i++) {

        if (accumulator[i] != 0) {
            // convert the point in the accumulator space in a line in the
            // normal space
            // to do so: compute 2 point of the line and draw it
            double thetaRad = theta * pi / 180;
            double a = cos(thetaRad);
            double b = sin(thetaRad);
            int x0 = a * rho;
            int y0 = b * rho;
            int x1 = x0 + 3000 * (-b);
            int y1 = y0 + 3000 * a;
            int x2 = x0 - 3000 * (-b);
            int y2 = y0 - 3000 * a;

            ComputeLine(normalSpace, w, h, x1, y1, x2, y2);
        }

        theta++;
        if (theta == maxTheta) {
            theta = 0;
            rho++;
        }
    }
    return normalSpace;
}

void ComputeLine(unsigned int *normalSpace, long int w, long int h, long int x1,
                 long int y1, long int x2, long int y2) {
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
        if (x >= 0 && x < w && y >= 0 && y < h)
            normalSpace[y * w + x] += 1;
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



//----------------------------UTILS--------------------------
void PrintMat(unsigned int *accumulator) {
    size_t count = 0;
    for (size_t i = 0; i < accumulatorSize; i++) {
        if (i % maxTheta == 0)
            printf("\n");

        unsigned int val = accumulator[i];
        if (val >= minPeak) {
            printf("\033[0;31m");
            printf("%3u ", accumulator[i]);
            count++;
        } else if (val >= 1) {
            printf("\033[0m");
            printf("%3u ", accumulator[i]);
            count++;
        } else {
            printf("\033[0m");
            printf("   ");
        }
    }
    printf("\n");
    printf("lines detectes: %lu\n", count);
}

void DrawLines(SDL_Surface *surface, unsigned int *accumulator, int *pixels) {
    Uint32 color = SDL_MapRGB(surface->format, 255, 0, 0);

    int rho = 0;
    int theta = 0;
    for (size_t i = 0; i < accumulatorSize; i++) {


        unsigned int val = accumulator[i];
        // A peak has its value greater than minPeak and gretaer or equal than
        // its 4 closest neighbours first compare to the peak then left / right
        // / top / bottom
        if (val != 0) {
            // Drawing the corresponding lines
            double thetaRad = theta * pi / 180;
            double a = cos(thetaRad);
            double b = sin(thetaRad);
            int x0 = a * rho;
            int y0 = b * rho;
            int x1 = x0 + 3000 * (-b);
            int y1 = y0 + 3000 * a;
            int x2 = x0 - 3000 * (-b);
            int y2 = y0 - 3000 * a;

            DrawLine(pixels, surface->w, surface->h, x1, y1, x2, y2, color);
        }

        theta++;
        if (theta == maxTheta){
            theta = 0;
            rho++;
        }
    }
}

void DrawLine(int *pixels, long int w, long int h, long int x1, long int y1,
              long int x2, long int y2, Uint32 color) {
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
        if (x >= 0 && x < w && y >= 0 && y < h)
            pixels[y * w + x] = color;
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

void DrawIntersections(SDL_Surface *surface, unsigned int *space) {
    int w = surface->w;
    int h = surface->h;

    Uint32 color = SDL_MapRGB(surface->format, 255, 0, 0);
    int x = 0, y = 0;
    for (long i = 0; i < w * h; i++) {
        unsigned int val = space[i];

        if (val >= 2) {
            SetPixelData(surface, x, y, color);
        }

        x++;
        if (x == w) {
            y++;
            x = 0;
        }
    }
}
