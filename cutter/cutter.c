#include <stdlib.h>
#include <err.h>
#include <SDL2/SDL_image.h>

#include "../utils.h"
#include "cutter.h"


SDL_Surface *surf;
int maxDist;

int minAverage = 200;
int minPic = 250;

float cosArray[180];
float sinArray[180];


void DetectLines() {
    CreateAnglesArray();



    //Load the image
    surf = LoadImage("DataSample/cutter/og1rotate.png");
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
            if (average > minAverage){
                FillAcumulator(x, y, space);
            }
        }
    }
    

    //PrintMat(space);


    // Searching for the highest values
    int y = -maxDist;
    for(int i = 0; i < maxDist * 360; i ++){
        if (space[i] >= minPic){
            DrawLine(surf, i%180-90, y);
        }

        if(i%180 == 0) y++;
    }
    
    IMG_SavePNG(surf, "test.png");

    // To avoid memory leak
    free(space);
    printf("memory freed\n");
}




void FillAcumulator(int x, int y, unsigned char * space){
    for (int i = 0; i < 180; i++){
        int rho = x*cosArray[i] + y*sinArray[i] + maxDist;
        space[rho*180 + i] += 1;
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
void DrawLine(SDL_Surface * surf, int teta, int rho){
    float angle = teta * 3.141592 / 180; 
    float costeta = cos(angle);
    float sinteta = sin(angle);

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



void PrintMat(unsigned char * space){
    for(int i = 0; i < maxDist * 360; i ++){
        if(i % 180 == 0)
            printf("\n");
        if(space[i] >= minPic){
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
}
