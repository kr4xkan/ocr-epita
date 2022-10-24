#pragma once

#include <SDL2/SDL.h>

void shuffle(int *array, size_t n);

Uint32 GetPixelData(const SDL_Surface *surface, int x, int y);

void GetPixelColor(const SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g,
                   Uint8 *b);
