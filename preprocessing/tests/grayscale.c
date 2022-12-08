#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Surface *load_image(const char *path) {
    SDL_Surface *surf = IMG_Load(path);
    SDL_Surface *res =
        SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(surf);
    return res;
}

// Converts a colored pixel into grayscale.
//
// pixel_color: Color of the pixel to convert in the RGB format.
// format: Format of the pixel used by the surface.
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

int main(int argc, char **argv) {
    // - Create a surface from the colored image.
    SDL_Surface *surface = load_image(argv[1]);

    // - Convert the surface into grayscale.
    surface_to_grayscale(surface);

    IMG_SavePNG(surface, "out.png");

    SDL_FreeSurface(surface);
    return EXIT_SUCCESS;
}
