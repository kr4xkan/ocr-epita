#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Surface *load_image(const char *path) {
    SDL_Surface *surf = IMG_Load(path);
    SDL_Surface *res =
        SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGB888, 0);
    return res;
}

Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat *format) {
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    Uint8 average = 0.3 * r + 0.59 * g + 0.11 * b;
    Uint32 color = SDL_MapRGB(format, average, average, average);
    return color;
}

void surface_to_grayscale(SDL_Surface *surface) {
    Uint32 *pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat *format = surface->format;
    SDL_LockSurface(surface);
    for (int i = 0; i < len; i++) {
        pixels[i] = pixel_to_grayscale(pixels[i], format);
    }
    SDL_UnlockSurface(surface);
}

int otsu(SDL_Surface *img, int w, int h) {
    Uint32 *pixels = img->pixels;
    float histo[256];
    int nbpix = w * h;
    int threshold = 0;
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            Uint8 r, g, b;
            Uint32 pixel = pixels[y * w + x];
            SDL_GetRGB(pixel, img->format, &r, &g, &b);
            histo[g] += 1;
        }
    }

    float w1 = 0; // sum of all expectations
    for (int i = 0; i <= 255; i++) {
        w1 += i * ((int)histo[i]);
    }

    float w2 = 0;     // expectation sum 2
    int n1 = 0;       // histo value for i
    int n2;           // histo value for all others
    float m1;         // mean value 1
    float m2;         // mean value 2
    float var;        // each value var to compare with maxvar
    float maxvar = 0; // max variance : result

    for (int i = 0; i <= 255; i++) // calcul of the best threshold : the one who
                                   // as the greatest variance.
    {
        n1 += histo[i];
        n2 = nbpix - n1;
        w2 += (float)(i * ((int)histo[i]));
        m1 = w2 / n1;        // mean 1
        m2 = (w1 - w2) / n2; // mean 2
        var = (float)n1 * (float)n2 * (m1 - m2) * (m1 - m2);
        if (var > maxvar) {
            maxvar = var;
            threshold = i;
        }
    }
    return threshold + 11;
}

void dumb_bin(SDL_Surface *surface) {
    Uint32 *pixels = surface->pixels;
    int w = surface->w;
    int h = surface->h;
    int threshold = otsu(surface, w, h);
    for (size_t i = 0; i < h; i++) {
        for (size_t k = 0; k < w; k++) {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[i * w + k], surface->format, &r, &g, &b);
            if (r >= threshold) {
                pixels[w * i + k] = SDL_MapRGB(surface->format, 255, 255, 255);
            } else
                pixels[w * i + k] = SDL_MapRGB(surface->format, 0, 0, 0);
        }
    }
}

int main(int argc, char **argv) {
    SDL_Surface *surface = load_image(argv[1]);

    surface_to_grayscale(surface);

    IMG_SavePNG(surface, "gray.png");

    SDL_Surface *surf2 = load_image("gray.png");

    dumb_bin(surf2);

    IMG_SavePNG(surf2, "dumb.png");

    SDL_FreeSurface(surface);

    return EXIT_SUCCESS;
}
