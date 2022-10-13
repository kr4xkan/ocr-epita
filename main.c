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

    unsigned int *space = DetectLines(surface);

    //DrawLines(surface, space, surface->pixels);

    //PrintMat(space);


    SDL_Surface *rotated = CheckRotation(surface, space);
    if (rotated != NULL){
        unsigned int *space2 = DetectLines(rotated);
        //DrawLines(rotated, space2, rotated->pixels);


        IMG_SavePNG(rotated, "rotated.png");

        free(space2);
        SDL_FreeSurface(rotated);
    }


    //IMG_SavePNG(surface, "test.png");
    
    free(space);
    SDL_FreeSurface(surface);



    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds
 
    printf("%f seconds to execute \n", time_taken);
    return 0;
}
