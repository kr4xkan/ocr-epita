#pragma once
#include <SDL2/SDL_image.h>



// -----------------------------Crop--------------------------------------
typedef struct Intersection{
    unsigned int x; 
    unsigned int y; 
} Intersection;

Intersection* DetectIntersections(SDL_Surface *surface, unsigned int *normalSpace, size_t *length);
//size_t GapSize(SDL_Surface *surface, Intersection *coords, size_t len);


SDL_Surface *CropSurface(SDL_Surface *surface, Intersection current, int width,
        int height);
void CropSquares(SDL_Surface *surface, Intersection *coords, size_t len);


