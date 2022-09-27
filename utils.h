#pragma once

#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

Uint32 GetPixelData(const SDL_Surface *surface, int x, int y);

void GetPixelColor(const SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g,
        Uint8 *b);


SDL_Surface* LoadImage(const char* path);
