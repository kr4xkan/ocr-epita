#pragma once
#include <SDL2/SDL_image.h>


// -----------------------------Lines--------------------------------------
unsigned int* DetectLines(SDL_Surface *surface);
void FillAcumulator(SDL_Surface *surface, unsigned int *accumulator);
unsigned int FindMinPeak(unsigned int * accumulator, int accumulatorSize);
void FilterLines(unsigned int * accumulator, int accumulatorSize);
int CheckPeak(unsigned int *accumulator, int accumulatorSize, int i, unsigned int val);
int AlreadyExist(int theta, int rho, unsigned int rhoValues[], unsigned int thetaValues[], size_t len, int maxGap);


// -----------------------------Rotation--------------------------------------
SDL_Surface* CheckRotation(SDL_Surface *surface, unsigned int *accumulator);
SDL_Surface* RotateSurface(SDL_Surface* origine, float angle);


// -----------------------------Intersections--------------------------------------
unsigned int* DetectIntersections(SDL_Surface *surface, unsigned int *accumulator);
void ComputeLine(unsigned int *linesArray, long int w, long int h, 
        long int x1, long int y1, long int x2, long int y2);


// -----------------------------Crop--------------------------------------
void CropSquares(SDL_Surface *surface, unsigned int *normalSpace);
SDL_Surface* CropSurface(SDL_Surface* surface, int x, int y, int width, int height);



// -----------------------------UTILS-------------------------------------
void DrawIntersections(SDL_Surface *surface, unsigned int *accumulator);
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
