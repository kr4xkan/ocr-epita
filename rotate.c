#include "err.h"
#include "cutter/cutter.h"
#include "utils.h"

int main(int argc, char **argv) {
    if (argc != 3)
        errx(1, "\nUsage: ./rotate <image_path> <angle>\nExample: ./rotate image.png 20");


    SDL_Surface *surface = LoadImage(argv[1]);
    if (!surface) 
        errx(1, "Could not load image");

    float angle = atof(argv[2]);
    

    SDL_Surface *rotated = RotateSurface(surface, angle);

    char *new_str = malloc((strlen(argv[1]) + 3) * sizeof(char));
    strcpy(new_str, "rt_");
    strcat(new_str, argv[1]);
    IMG_SavePNG(rotated, new_str);

    return 0;
}
