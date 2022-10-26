#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>

void shuffle(int *array, size_t n);

Uint32 GetPixelData(const SDL_Surface *surface, int x, int y);
void SetPixelData(SDL_Surface *surface, int x, int y, Uint32 pixel);

void GetPixelColor(const SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g,
                   Uint8 *b);

SDL_Surface *LoadImage(const char *path);
