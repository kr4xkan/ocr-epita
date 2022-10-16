#include "cutter/cutter.h"
#include "preprocessing/preprocessing.h"
#include "neural-net/neural-net.h"
#include "solver/solver.h"
#include "ui/ui.h"
#include "utils.h"

#include <time.h> 

int main() {
    clock_t t = clock();
    

    //Load the surface
    SDL_Surface *surface = LoadImage("DataSample/cutter/og1rotated.png");
    if (!surface) 
        errx(1, "Could not load image");


    //Detect the lines
    unsigned int *accumulator = DetectLines(surface);

    //DrawLines(surface, accumulator, surface->pixels);

    //PrintMat(accumulator);


    //Rotate the image if necessary
    SDL_Surface *surfaceRotated = CheckRotation(surface, accumulator);
    if (surfaceRotated != NULL){
        unsigned int *accumulatorRotated = DetectLines(surfaceRotated);
        //DrawLines(surfaceRotated, accumulatorRotated, surfaceRotated->pixels);


        //IMG_SavePNG(surfaceRotated, "surfaceRotated.png");

        CheckRotation(surfaceRotated, accumulatorRotated);


        printf("before (rotated)\n");
        unsigned int *spaceRotated = DetectIntersections(surfaceRotated, accumulatorRotated);
        printf("after\n");


        test(surfaceRotated, spaceRotated);
        IMG_SavePNG(surfaceRotated, "surfaceRotated.png");



        free(spaceRotated);
        free(accumulatorRotated);
        SDL_FreeSurface(surfaceRotated);
    }
    else {
        printf("before (not rotated)\n");
        unsigned int *space = DetectIntersections(surface, accumulator);
        printf("after\n");
        free(space);

    }




    //IMG_SavePNG(surface, "test.png");
    
    free(accumulator);
    SDL_FreeSurface(surface);



    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
 
    printf("%f seconds to execute \n", time_taken);
    return 0;
}
