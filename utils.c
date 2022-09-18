#include "utils.h"

Uint32 GetPixelData(const SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;

    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        return *p;
    case 2:
        return *(Uint16 *)p;
    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
        return *(Uint32 *)p;
    default:
        return 0;
    }
}

void GetPixelColor(const SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g,
                   Uint8 *b) {

    SDL_GetRGB(GetPixelData(surface, x, y), surface->format, (Uint8 *)r,
               (Uint8 *)g, (Uint8 *)b);
}
