#pragma once
#include <SDL2/SDL_image.h>

unsigned int* DetectLines(SDL_Surface *surface);

void FillAcumulator(SDL_Surface *surface, unsigned int *space);

unsigned int FindMinPeak(unsigned int * space, int spaceSize);

SDL_Surface* CheckRotation(SDL_Surface *surface, unsigned int *space);

SDL_Surface* RotateSurface(SDL_Surface* origine, float angle);




void PrintMat(unsigned int * space);
void DrawLines(SDL_Surface *surface, unsigned int *space, int *pixels);
void DrawLine(int *pixels,
          long int w,
          long int h,
          long int x1,
          long int y1,
          long int x2,
          long int y2,
          Uint32 color);
