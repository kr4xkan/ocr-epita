#pragma once
#include <SDL2/SDL_image.h>



// -----------------------------Crop--------------------------------------
typedef struct Intersection{
    unsigned int x; 
    unsigned int y; 
} Intersection;


Intersection* FindIntersections(SDL_Surface *surface, unsigned int *normalSpace, size_t vertLen, size_t horiLen);
int DontAdd(Intersection *coords, size_t x, size_t y, size_t nbIntersections);


void CropSquares(SDL_Surface *surface, Intersection *coords, size_t vertLen, size_t horiLen);
SDL_Surface *CropSurface(SDL_Surface *surface, Intersection current, int width,
        int height);

void ManualCrop(SDL_Surface *surface, Intersection topLeft, Intersection topRight, Intersection bottomLeft, Intersection bottomRight);
Intersection *FindPoints(Intersection a, Intersection b);

