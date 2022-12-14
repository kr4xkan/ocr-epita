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

void SetPixelData(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:

        *(Uint32 *)p = pixel;
        break;
    }
}

void GetPixelColor(const SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g,
                   Uint8 *b) {

    SDL_GetRGB(GetPixelData(surface, x, y), surface->format, (Uint8 *)r,
               (Uint8 *)g, (Uint8 *)b);
}

SDL_Surface *LoadImage(const char *path) {
    SDL_Surface *tmp = IMG_Load(path);
    if (tmp == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface *res = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGB888, 0);
    if (res == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(tmp);
    return res;
}
