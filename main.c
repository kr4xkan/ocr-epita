#include "cutter/cutter.h"
#include "neural-net/neural-net.h"
#include "preprocessing/preprocessing.h"
#include "solver/solver.h"
#include "ui/ui.h"
#include "utils.h"

#include <time.h>

int main() {
    printf("\n");
    clock_t t = clock();

    // WARNING: NEVER USE TWO ACCUMULATOR AT THE SAME TIME
    // SOME VALUES ARE COMPUTED WITH DetectLines AND WILL CHANGE IF USE WITH
    // ANOTHER ACCUMULATOR

    // Load the surface
    SDL_Surface *surface = LoadImage("DataSample/cutter/og1rotated.png");
    if (!surface)
        errx(1, "Could not load image");

    unsigned int *accumulator = DetectLines(surface);

    // PrintMat(accumulator);

    // Rotate the image if necessary
    SDL_Surface *surfaceRotated = CheckRotation(surface, accumulator);

    if (surfaceRotated != NULL) {
        unsigned int *accumulatorRotated = DetectLines(surfaceRotated);
        unsigned int *spaceRotated =
            DetectIntersections(surfaceRotated, accumulatorRotated);

        DrawLines(surfaceRotated, accumulatorRotated, surfaceRotated->pixels);
        DrawIntersections(surfaceRotated, spaceRotated);
        IMG_SavePNG(surfaceRotated, "surface.png");

        // CropSquares(surfaceRotated, spaceRotated);

        free(spaceRotated);
        free(accumulatorRotated);
        SDL_FreeSurface(surfaceRotated);
    } else {
        unsigned int *space = DetectIntersections(surface, accumulator);

        DrawLines(surface, accumulator, surface->pixels);
        DrawIntersections(surface, space);
        IMG_SavePNG(surface, "surface.png");

        free(space);
    }

    free(accumulator);
    SDL_FreeSurface(surface);

    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC; // in seconds

    printf("\n%f seconds to execute \n", time_taken);
    return 0;
}
