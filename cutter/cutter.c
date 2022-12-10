#include <SDL2/SDL_surface.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cutter.h"

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



SDL_Surface **ManualCutter(SDL_Surface *ogSurf, Intersection *corners){
    return ManualCrop(ogSurf, corners);
}



SDL_Surface **AutoCutter(SDL_Surface *ogSurf, SDL_Surface *binSurf) {
    clock_t t = clock();
    
    SDL_Surface **res;


    unsigned int *accumulator = CreateAccumulator(binSurf);
    Line *lines = DetectLines(accumulator);

    int angle = 0;
    SDL_Surface *binSurfRotated = CheckRotation(binSurf, accumulator, &angle);

    if (binSurfRotated == NULL) {
        size_t vertLen = 0, horiLen = 0;
        lines = FilterLines(accumulator, lines, &vertLen, &horiLen);
        printf("lines detected : vertical:%lu horizontal:%lu\n", vertLen, horiLen);

        unsigned int *space = CreateSpace(binSurf, lines);
        Intersection *intersections = FindIntersections(binSurf, space, vertLen, horiLen);
        
        res = CropSquares(ogSurf, intersections, vertLen, horiLen);

        DrawLines(ogSurf, ogSurf->pixels, lines, vertLen*horiLen);
        //DrawIntersections(ogSurf, intersections, 10*10);
        IMG_SavePNG(ogSurf, "result.png");

        
        free(space);
        free(intersections);
    }
    else{
        SDL_Surface *ogSurfRotated = RotateSurface(ogSurf, angle);

        unsigned int *accumulatorRotated = CreateAccumulator(binSurfRotated);
        Line *linesRotated = DetectLines(accumulatorRotated);

        size_t vertLen = 0, horiLen = 0;
        linesRotated = FilterLines(accumulatorRotated, linesRotated, &vertLen, &horiLen);
        printf("lines detected : vertical:%lu horizontal:%lu\n", vertLen, horiLen);
    
        unsigned int *spaceRotated = CreateSpace(binSurfRotated, linesRotated);
        Intersection *intersections = FindIntersections(binSurfRotated, spaceRotated, vertLen, horiLen);

        res = CropSquares(ogSurfRotated, intersections, vertLen, horiLen);

        DrawLines(ogSurfRotated, ogSurfRotated->pixels, linesRotated, vertLen*horiLen);
        //DrawIntersections(ogSurfRotated, intersections, vertLen*horiLen);
        IMG_SavePNG(ogSurfRotated, "result.png");


        free(accumulatorRotated);
        free(linesRotated);
        free(spaceRotated);
        free(intersections);
        SDL_FreeSurface(binSurfRotated);
        SDL_FreeSurface(ogSurfRotated);
    } 
    free(accumulator);
    free(lines);


    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds
    printf("\n%f seconds to execute \n", time_taken);
    return res;
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
    minPeak = FindMinPeak(accumulator);

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

unsigned int FindMinPeak(unsigned int *accumulator) {
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

Line* DetectLines(unsigned int *accumulator){
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
        if (CheckPeak(accumulator, i, val)) {
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

int CheckPeak(unsigned int *accumulator, size_t i, unsigned int val) {
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


Line* FilterLines(unsigned int *accumulator, Line* lines, size_t *vertLen, size_t *horiLen){

    size_t len = 0;
    while (lines[len].accuPos != accumulatorSize+1)
        len++;

    size_t histoSize = accumulatorSize/maxTheta;
    unsigned int *histoVert = calloc(histoSize, sizeof(unsigned int));
    unsigned int *histoHori = calloc(histoSize, sizeof(unsigned int));

    Line *vertLines = calloc(len, sizeof(Line));
    Line *horiLines = calloc(len, sizeof(Line));

    //Filter in two separate array vertical and horizontal lines
    //Remove the others
    unsigned int rho1 = -1, rho2 = -1;
    for (size_t i = 0; i < len; i++){
        Line line = lines[i];

        if (line.theta % 90 >= 2 && line.theta % 90 <= 88) 
            accumulator[line.accuPos] = 0;            
        else{
            if(line.theta < 2 || line.theta > 178){
                vertLines[*vertLen] = line;
                (*vertLen)++;
                if (*vertLen == 1)
                    rho1 = line.rho;
                else{
                    histoVert[line.rho - rho1] += 1;
                    rho1 = line.rho;
                }
            }
            else{
                horiLines[*horiLen] = line;
                (*horiLen)++;
                if (*horiLen == 1)
                    rho2 = line.rho;
                else{
                    histoHori[line.rho - rho2] += 1;
                    rho2 = line.rho;
                }
            }
        }
    }


    size_t range = 5;
    size_t vertCapacity = *vertLen, horiCapacity = *horiLen;
    while (range > 0 && (*horiLen > 10 || *vertLen > 10)){
        size_t gap = FindGap(histoVert, histoSize, range);
        Remove(accumulator, vertLines, vertCapacity, vertLen, gap);
        gap = FindGap(histoHori, histoSize, range);
        Remove(accumulator, horiLines, horiCapacity, horiLen, gap);
        range--;
    }
    
    Line *newLines = malloc(((*vertLen)*(*horiLen)+1)*sizeof(Line));
    size_t i = 0, j = 0;
    while (i < vertCapacity){
        if (vertLines[i].value != 0){
            if (j >= 10){
                (*vertLen)--;
                accumulator[vertLines[i].accuPos] = 0;
            }
            else{
                newLines[j] = vertLines[i];
                j++;
            }
        }
        i++;
    }
    i = 0;
    while (i < horiCapacity){
        if (horiLines[i].value != 0){
            if (j >= *vertLen + 10){
                (*horiLen)--;
                accumulator[horiLines[i].accuPos] = 0;
            }
            else{
                newLines[j] = horiLines[i];
                j++;
            }
        }
        i++;
    }
    newLines[j].value = 0;


    free(histoVert);
    free(histoHori);
    free(vertLines);
    free(horiLines);
    free(lines);
    return newLines;
}



size_t FindGap(unsigned int *histo, size_t histoSize, size_t range){
    size_t current = 0;
    for (size_t i = 0; i <= range*2; i++)
        current += histo[i];

    size_t gap = range;
    size_t max = current;
    for (size_t i = range+2; i < histoSize-range; i++){
        current -= histo[i-range-1];
        current += histo[i+range];
        if (current > max){
            gap = i;
            max = current;
        }
    }
    return gap;
}


void Remove(unsigned int *accumulator, Line *lines, size_t capacity, size_t *size, size_t gap){
    size_t dGap = gap/2;
    while (dGap > 0 && *size > 10){
        //Vertical Lines
        char inSudoku = 0;
        Line *prev = &lines[0];
        for (size_t i = 1; i < capacity; i++){
            Line *curr = &lines[i];
            if (curr->value == 0 || prev->value == 0){
                continue;
            }

            size_t dRho = (curr->rho - prev->rho) % gap;
            if (dRho > dGap && dRho < gap-dGap){
                if (!inSudoku){
                    prev->value = 0;
                    accumulator[prev->accuPos] = 0;
                }
                else {
                    curr->value = 0;
                    accumulator[curr->accuPos] = 0;
                }
                (*size)--;
            }
            else {
                inSudoku = 1;
            }
            prev = curr;
        }
        dGap--;
    }
}





SDL_Surface *CheckRotation(SDL_Surface *surface, unsigned int *accumulator, int *angleDegree) {
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
    *angleDegree = angle;
    if (abs(angle) > 1) {
        if (angle >= 45) {
            printf("Rotate %i° Clockwise\n", 90 - angle);
            rotated = RotateSurface(surface, -90 + angle);
    printf("2 %d\n", angle);
        } else {
            printf("Rotate %i° CounterClockwise\n", angle);
            rotated = RotateSurface(surface, angle);
    printf("3 %d\n", angle);
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




unsigned int *CreateSpace(SDL_Surface *surface,
                                  Line* lines) {
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

    Line *current = lines;
    while (current->value != 0){
        double thetaRad = current->theta * pi / 180;
        double a = cos(thetaRad);
        double b = sin(thetaRad);
        int x0 = a * current->rho;
        int y0 = b * current->rho;
        int x1 = x0 + 3000 * (-b);
        int y1 = y0 + 3000 * a;
        int x2 = x0 - 3000 * (-b);
        int y2 = y0 - 3000 * a;

        ComputeLine(normalSpace, w, h, x1, y1, x2, y2);
        current+=1;
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
}

void DrawLines(SDL_Surface *surface, int *pixels, Line *lines, size_t len){
    Uint32 color = SDL_MapRGB(surface->format, 255, 0, 0);

    for (size_t i = 0; i < len; i++) {
        Line line = lines[i];


        double thetaRad = line.theta * pi / 180;
        double a = cos(thetaRad);
        double b = sin(thetaRad);
        int x0 = a * line.rho;
        int y0 = b * line.rho;
        int x1 = x0 + 3000 * (-b);
        int y1 = y0 + 3000 * a;
        int x2 = x0 - 3000 * (-b);
        int y2 = y0 - 3000 * a;

        DrawLine(pixels, surface->w, surface->h, x1, y1, x2, y2, color);
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

void DrawIntersections(SDL_Surface *surface, Intersection *coords, size_t len){
    int w = surface->w;
    int h = surface->h;

    Uint32 color = SDL_MapRGB(surface->format, 0, 0, 255);
    for (size_t i = 0; i < len; i++) {
        int x = (int)coords[i].x;
        int y = (int)coords[i].y;
        for (int dx = -5; dx <= 5; dx++){
            for (int dy = -5; dy <= 5; dy++){
                if (x+dx >= 0 && x+dx < w && y+dy >=0 && y+dy < h){
                    SetPixelData(surface, x+dx, y+dy, color);
                }
            }
        }
    }
}
