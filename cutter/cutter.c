#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL_image.h>

#include "../utils.h"
#include "cutter.h"


int maxDist;

int minAverage = 200;
int minPeak;

// Array of teta values
float cosArray[180];
float sinArray[180];


int jsp = 0;

void DetectLines() {
    // Creates all the values that teta will become in the loops
    CreateAnglesArray();

    
    SDL_Surface *surface;
    //Load the image
    surface = LoadImage("DataSample/cutter/og1inverted.png");
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

            jsp ++;
            GetPixelColor(surface, x, y, &r, &g, &b);
            unsigned char average = (r + g + b) / 3;
            if (average > minAverage){
                FillAcumulator(x, y, space);
            }
        }
    }
    

    PeakDetection(space, surface);
    //PrintMat(space);


    IMG_SavePNG(surface, "test.png");

    // To avoid memory leak
    free(space);
    SDL_FreeSurface(surface);
    printf("memory freed\n");
    printf("%i\n", jsp);
}




void FillAcumulator(int x, int y, unsigned char * space){
    for (int i = 0; i < 180; i++){
        int rho = x*cosArray[i] + y*sinArray[i] + maxDist;
        space[rho*180 + i] += 1;
    }
}



void PeakDetection(unsigned char * space, SDL_Surface * surface){
    unsigned char maxPeak = 0;
    for (int i = 0; i < maxDist*360; i++){
        if (space[i] > maxPeak)
            maxPeak = space[i];
    }


    int y = -maxDist;
    minPeak = maxPeak * 1;
    for (int i = 181; i < maxDist*360 - 181; i++){
        unsigned char val = space[i];
        if (val >= minPeak)
            if (val > space[i-1] && val > space[i+1] && val > space[i-180] && val > space[i+180])
                DrawLine(surface, i%180-90, y);

        if(i%180 == 0) y++;
    }
}

void CreateAnglesArray(){
    int i = 0;
    for (int teta = -90; teta < 90; teta++){
        float angle = teta * 3.141592 / 180; 
        cosArray[i] = cos(angle);
        sinArray[i] = sin(angle);
        i++;
    }
    
}

//----------------------------------UTILS------------------------------
void DrawLine(SDL_Surface * surface, int teta, int rho){
    float angle = teta * 3.141592 / 180; 
    float costeta = cos(angle);
    float sinteta = sin(angle);

    SDL_Rect r;
    r.w = 1;
    r.h = 1;
    for (int x = 0; x < surface->w; x++){
        int y = (rho - x*costeta) / sinteta;
        r.x = x;
        r.y = y;
        SDL_FillRect(surface, &r, SDL_MapRGB(surface->format, 255, 0, 0));
    }
    
}



void PrintMat(unsigned char * space){
    for(int i = 0; i < maxDist * 360; i ++){
        if(i % 180 == 0)
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
