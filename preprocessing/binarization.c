#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surf = IMG_Load(path);
    SDL_Surface* res = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGB888, 0);
    return res;
}

/*void binarize_square(Uint32* pixels, int Hmin, int Wmin, int Hmax, int Wmax, int width, int height)
{
    Uint32 mid = 0;
    int nb = (Wmax-Wmin) * (Hmax-Hmin);
    for (int i = Hmin; i < Hmax; i++)
    {
        for (int k = Wmin; k < Wmax; k++)
        {
            mid += pixels[i*width + k];
        }
    }
    Uint32 threshold = mid / nb;
    for (int i = Hmin; i < Hmax; i++)
    {
        for (int k = Wmin; k < Wmax; k++)
        {
            if (pixels[i*width + k] > threshold)
                pixels[i*width + k] = SDL_MapRGB(SDL_PIXELFORMAT_RGB888, 0, 0, 0);
            else
                pixels[i*width + k] = SDL_MapRGB(SDL_PIXELFORMAT_RGB888, 255, 255, 255);
        }
    }
}*/


Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    Uint8 average = 0.3*r + 0.59*g + 0.11*b;
    Uint32 color = SDL_MapRGB(format, average, average, average);
    return color;
}

void surface_to_grayscale(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;
    SDL_LockSurface(surface);
    for (int i = 0; i < len; i++)
    {
        pixels[i] = pixel_to_grayscale(pixels[i], format);
    }
    SDL_UnlockSurface(surface);
}


int otsu(SDL_Surface* img, int Hmin, int Wmin, int Hmax, int Wmax, int w, int h)
{
    Uint32 *pixels = img->pixels;
    float histo[256];
    int nbpix = (Wmax-Wmin) * (Hmax-Hmin);
    int threshold = 0;
    for (int x = Wmin; x < Wmax; x++)
    {
        for (int y = Hmin; y < Hmax; y++)
        {
            Uint8 r, g, b;
            Uint32 pixel = pixels[y*w + x];
            SDL_GetRGB(pixel, img->format, &r, &g, &b);
            histo[g]+=1;
        }
    }

    float w1 = 0; //sum of all expectations
    for (int i = 0; i <= 255; i++)
    {
        w1 += i * ((int)histo[i]);
    }

    float w2 = 0; //expectation sum 2
    int n1 = 0; //histo value for i
    int n2; //histo value for all others
    float m1; //mean value 1
    float m2; //mean value 2
    float var; //each value var to compare with maxvar
    float maxvar = 0; //max variance : result

    for (int i = 0 ; i <= 255 ; i++) //calcul of the best threshold : the one who as the greatest variance.
    {
        n1 += histo[i];
        n2 = nbpix - n1;
        w2 += (float) (i * ((int)histo[i]));
        m1 = w2 / n1; //mean 1
        m2 = (w1 - w2) / n2; //mean 2
        var = (float) n1 * (float) n2 * (m1 - m2) * (m1 - m2);
        if (var > maxvar)
        {
            maxvar = var;
            threshold = i;
        }
    }
    return threshold-11;
}

void binarize_square(SDL_PixelFormat *format, Uint32* pixels, int threshold, int Hmin, int Wmin, int Hmax, int Wmax,
                     int w, int h)
{
    for (int x = Wmin; x < Wmax; x++)
    {
        for (int y = Hmin; y < Hmax; y++)
        {
            Uint8 r,g,b;
            SDL_GetRGB(pixels[x*w+y],format, &r,&g,&b);
            //printf("%u\n", pixels[i*w+k]);
            if (r > threshold) {
                pixels[w * x + y] = SDL_MapRGB(format, 255, 255, 255);
            }
            else
                pixels[w*x+y] = SDL_MapRGB(format, 0, 0, 0);
        }
    }
}

void binarize_surface(SDL_Surface* surface) {
    int w = surface->w;
    int h = surface->h;
    Uint32 *pixels = surface->pixels;
    int h_list[6] = {0, 0, 0, 0, 0, h};
    int w_list[6] = {0, 0, 0, 0, 0, w};
    int cut_h = h / 5;
    int cut_w = w / 5;
    for (int i = 1; i < 5; i++) {
        h_list[i] += cut_h * i;
        w_list[i] += cut_w * i;
    }
    for (int k = 1; k < 6; k++) {
        for (int j = 1; j < 6; j++)
        {
            int threshold = otsu(surface, h_list[j-1], w_list[k-1], h_list[j], w_list[k], w, h);
            binarize_square(surface->format, pixels, threshold, h_list[j-1], w_list[k-1], h_list[j],
                            w_list[k], w, h);
        }

    }

}

int main(int argc, char** argv){
    SDL_Surface* surface = load_image(argv[1]);

    surface_to_grayscale(surface);

    binarize_surface(surface);

    IMG_SavePNG(surface, "bin.png");

    SDL_FreeSurface(surface);

    return EXIT_SUCCESS;
}
//void binarize_square(Uint32* pixels, int Hmin, int Wmin, Hmax, int Wmax, Uint32 mid);
