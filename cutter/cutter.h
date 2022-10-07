#pragma once
#include <SDL2/SDL_image.h>

void DetectLines();
void FillAcumulator(int x, int y, unsigned int * space);
void PrintMat(unsigned int * space);
void DrawLine(int *pixels,
          long int w,
          long int h,
          long int x1,
          long int y1,
          long int x2,
          long int y2,
          Uint32 color);
