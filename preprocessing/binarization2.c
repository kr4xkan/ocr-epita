#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surf = IMG_Load(path);
    SDL_Surface* res = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGB888, 0);
    return res;
}

SDL_Surface* dumb_bin(SDL_Surface *surface){
    Uint32 *pixels = surface->pixels;
    size_t w = surface->w;
    size_t h = surface->h;
    size_t nb = w*h;
    Uint32 mid = 0;
    for (size_t i = 0; i < h; i++)
    {
        for(size_t k = 0; k < w; k++)
        {
            mid += pixels[w*i + k];
        }
    }
    Uint32 threshold = mid / nb;
    SDL_Surface *res = SDL_CreateRGBSurface(0, w, h, 32,0,0,0,0);
    for (size_t i = 0; i < h; i++)
    {
        for(size_t k = 0; k < w; k++)
        {

            if (pixels[i*w+k] <= threshold)
                pixels[w*i+k] = SDL_MapRGB(surface->format, 255, 255, 255);
            else
                pixels[w*i+k] = SDL_MapRGB(surface->format, 0, 0, 0);
        }
    }
    return res;
}

int main(int argc, char** argv){
    SDL_Surface* surface = load_image(argv[1]);

    SDL_Surface *res = dumb_bin(surface);

    IMG_SavePNG(res, "dumb.png");

    SDL_FreeSurface(surface);
    SDL_FreeSurface(res);

    return EXIT_SUCCESS;
}