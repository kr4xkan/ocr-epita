#pragma once
#include <SDL2/SDL_image.h>

void DetectLines();
void FillAcumulator(int x, int y, unsigned char * space);
void DrawLine(SDL_Surface * surf, int teta, int rho);
