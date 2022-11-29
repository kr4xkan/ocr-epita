#pragma once
#include <SDL2/SDL_image.h>



// -----------------------------Crop--------------------------------------
typedef struct Intersection{
    unsigned int x; 
    unsigned int y; 
} Intersection;

void CropSquares(SDL_Surface *surface, unsigned int *normalSpace);
SDL_Surface *CropSurface(SDL_Surface *surface, Intersection current, int width,
                                                    int height);
char TryCrop(SDL_Surface *surface, size_t len, Intersection *coords,
                                                    size_t nbIntersection);
char IsValid(Intersection *coords, size_t len, size_t x, size_t y, size_t gap);


size_t Ouaip(SDL_Surface *surface, Intersection *coords, size_t len);



