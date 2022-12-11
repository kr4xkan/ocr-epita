#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>

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

int otsu(SDL_Surface *img, int hmin, int wmin, int hmax, int wmax, int w) {
    Uint32 *pixels = img->pixels;
    double histo[256];
    for (size_t h = 0; h < 256; h++)
        histo[h] = 0;
    int nbpix = (wmax - wmin) * (hmax - hmin);
    int threshold = 0;
    for (int x = wmin; x < wmax; x++) {
        for (int y = hmin; y < hmax; y++) {
            Uint8 r, g, b;
            Uint32 pixel = pixels[y * w + x];
            SDL_GetRGB(pixel, img->format, &r, &g, &b);
            histo[g] += 1;
        }
    }

    double w1 = 0; // sum of all expectations
    for (int i = 0; i <= 255; i++) {
        w1 += i * ((int)histo[i]);
    }

    double w2 = 0;     // expectation sum 2
    long n1 = 0;       // histo value for i
    long n2;           // histo value for all others
    double m1;         // mean value 1
    double m2;         // mean value 2
    double var;        // each value var to compare with maxvar
    double maxvar = 0; // max variance : result

    for (int i = 0; i <= 255; i++) // calcul of the best threshold : the one who
                                   // as the greatest variance.
    {
        n1 += histo[i];
        n2 = nbpix - n1;
        w2 += (double)(i * ((int)histo[i]));
        m1 = w2 / n1;
        m2 = (w1 - w2) / n2;
        var = (double)n1 * (double)n2 * (m1 - m2) * (m1 - m2);
        if (var > maxvar) {
            maxvar = var;
            threshold = i;
        }
    }
    return threshold - 15;
}

void binarize_square(SDL_Surface *surface, int threshold, int Hmin, int Wmin,
                     int Hmax, int Wmax, int w) {
    Uint32 *pixels = surface->pixels;
    SDL_PixelFormat *format = surface->format;
    for (int x = Wmin; x < Wmax; x++) {
        for (int y = Hmin; y < Hmax; y++) {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[y * w + x], format, &r, &g, &b);
            // printf("%u\n", pixels[i*w+k]);
            if (r < threshold) {
                pixels[w * y + x] = SDL_MapRGB(format, 255, 255, 255);
            } else
                pixels[w * y + x] = SDL_MapRGB(format, 0, 0, 0);
        }
    }
}

void dumb_bin(SDL_Surface *surface, int cutter) {
    int w = surface->w;
    int h = surface->h;
    int *h_list = calloc(cutter + 1, sizeof(int));
    int *w_list = calloc(cutter + 1, sizeof(int));
    int cuth = h / cutter;
    int cutw = w / cutter;
    for (int i = 1; i < (cutter + 1); i++) {
        h_list[i] += cuth * i;
        w_list[i] += cutw * i;
    }

    for (int a = 1; a < (cutter + 1); a++) {
        for (int b = 1; b < (cutter + 1); b++) {
            int threshold = otsu(surface, h_list[b - 1], w_list[a - 1],
                                 h_list[b], w_list[a], w);
            binarize_square(surface, threshold, h_list[b - 1], w_list[a - 1],
                            h_list[b], w_list[a], w);
        }
    }
}

void binarization(SDL_Surface *surface, int otsu_size) {
    // SDL_Surface *surface = load_image(path);

    surface_to_grayscale(surface);

    IMG_SavePNG(surface, "grayscale.png");

    dumb_bin(surface, otsu_size);

    IMG_SavePNG(surface, "binary.png");
}

/*
int main(int argc, char **argv) {
    if (argc != 3) {
        errx(1, "Usage: ./preprocessing image_path mode");
    }

    binarization(argv[1], argv[2]);
}*/
