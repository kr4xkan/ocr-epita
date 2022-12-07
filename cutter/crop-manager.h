#pragma once
#include <SDL2/SDL_image.h>



// -----------------------------Crop--------------------------------------
typedef struct Intersection{
    unsigned int x; 
    unsigned int y; 
} Intersection;


Intersection* FindIntersections(SDL_Surface *surface, unsigned int *normalSpace, size_t vertLen, size_t horiLen);
int DontAdd(Intersection *coords, size_t x, size_t y, size_t nbIntersections);


SDL_Surface **CropSquares(SDL_Surface *surface, Intersection *coords, size_t vertLen, size_t horiLen);
SDL_Surface *CropSurface(SDL_Surface *surface, Intersection current, int width,
        int height);

SDL_Surface **ManualCrop(SDL_Surface *surface, Intersection *corners);
Intersection *FindPoints(Intersection a, Intersection b);


void StretchBlit(SDL_Surface* src,SDL_Surface* dest);
void StretchLinear(SDL_Surface* src,SDL_Surface* dest);
unsigned char GetPixelComp32(SDL_Surface* surface,int x,int y,int c);
void PutPixelComp32(SDL_Surface* surface,int x,int y,int c,unsigned char val);

