#pragma once
#include <SDL2/SDL_image.h>

void DetectLines();
unsigned int FindMinPeak(unsigned int * space, int spaceSize);
SDL_Surface* RotateSurface(SDL_Surface* origine, float angle);


void PrintMat(unsigned int * space, int spaceSize, unsigned int minPeak);
void DrawLine(int *pixels,
          long int w,
          long int h,
          long int x1,
          long int y1,
          long int x2,
          long int y2,
          Uint32 color);
