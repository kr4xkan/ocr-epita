#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surf = IMG_Load(path);
    SDL_Surface* res = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(surf);
    return res;
}

void binarize_square(Uint32* pixels, int Hmin, int Wmin, int Hmax, int Wmax, int width, int height)
{
    Uint32 mid = 0;
    Uint32 nb = 0;
    for (int i = Hmin; i < Hmax; i++)
    {
        for (int k = Wmin; k < Wmax; k++)
        {
            mid += pixels[i*height + k*width];
            nb+=1;
        }
    }
    Uint32 threshold = mid / nb;
    for (int i = Hmin; i < Hmax; i++)
    {
        for (int k = Wmin; k < Wmax; k++)
        {
            if (pixels[i*height + k*width] > threshold)
                pixels[i*height + k*width] = SDL_MapRGB(SDL_PIXELFORMAT_RGB888, 0, 0, 0);
            else
                pixels[i*height + k*width] = SDL_MapRGB(SDL_PIXELFORMAT_RGB888, 255, 255, 255);
        }
    }
}

void binarize_surface(SDL_Surface* surface) {
    int width = surface->w;
    int height = surface->h;
    //Uint32 *pixels = surface->pixels;
    Uint32 *pixels =malloc(width * height * sizeof(Uint32));
    *pixels = surface->pixels;
    int h_list[5] = {0, 0, 0, 0, 0};
    int w_list[5] = {0, 0, 0, 0, 0};
    int cut_h = height / 5;
    int cut_w = width / 5;
    for (int i = 1; i < 5; i++) {
        h_list[i] += cut_h * i;
        w_list[i] += cut_w * i;
    }
    for (int k = 0; k < 5; k++) {
        binarize_square(pixels, h_list[k], w_list[k], h_list[k + 1],
                        w_list[k + 1], width, height);
    }
}

int main(int argc, char** argv){
    SDL_Surface* surface = load_image(argv[1]);

    binarize_surface(surface);

    IMG_SavePNG(surface, "bin.png");

    SDL_FreeSurface(surface);

    return EXIT_SUCCESS;
}
//void binarize_square(Uint32* pixels, int Hmin, int Wmin, Hmax, int Wmax, Uint32 mid);
