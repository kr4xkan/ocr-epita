#pragma once
#include <SDL2/SDL_image.h>


//Take a surface as parameter and return the filled accumulator
unsigned int* DetectLines(SDL_Surface *surface);
void FillAcumulator(SDL_Surface *surface, unsigned int *accumulator);
unsigned int FindMinPeak(unsigned int * accumulator, int accumulatorSize);
void FilterLines(unsigned int * accumulator, int accumulatorSize);
int AlreadyExist(int theta, int rho, unsigned int rhoValues[], unsigned int thetaValues[], size_t len, int maxGap);


// Take a surface and its corresponding accumulator
// Return a surface if a rotation is needed, NULL else
SDL_Surface* CheckRotation(SDL_Surface *surface, unsigned int *accumulator);
SDL_Surface* RotateSurface(SDL_Surface* origine, float angle);


// Take a surface and its corresponding accumulator
// Return an array similar to surface->pixels but only wth the lines
unsigned int* DetectIntersections(SDL_Surface *surface, unsigned int *accumulator);
void ComputeLine(unsigned int *linesArray, long int w, long int h, 
        long int x1, long int y1, long int x2, long int y2);



void DrawIntersections(SDL_Surface *surface, unsigned int *accumulator);

// -----------------------------UTILS-------------------------------------
void PrintMat(unsigned int * accumulator);
void DrawLines(SDL_Surface *surface, unsigned int *accumulator, int *pixels);
void DrawLine(int *pixels,
          long int w,
          long int h,
          long int x1,
          long int y1,
          long int x2,
          long int y2,
          Uint32 color);
