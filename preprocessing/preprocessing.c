#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Updates the display.
//
// renderer: Renderer to draw on.
// texture: Texture that contains the image.
void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

// Event loop that calls the relevant event handler. 
//
// renderer: Renderer to draw on.
// colored: Texture that contains the colored image.
// grayscale: Texture that contains the grayscale image.
void event_loop(SDL_Renderer* renderer, SDL_Texture* grayscale) 
{
    SDL_Event event;
    SDL_Texture* t = grayscale;

    while (1)
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
            case SDL_QUIT:
                return;
        
            case SDL_WINDOWEVENT: 
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    draw(renderer, t);
                }
                break;
        }
    }
}
 
// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.
SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surf = IMG_Load(path);
    SDL_Surface* res = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(surf);
    return res;
}

// Converts a colored pixel into grayscale.
//
// pixel_color: Color of the pixel to convert in the RGB format.
// format: Format of the pixel used by the surface.
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

void binarize_surface(SDL_Surface* surface)
{
        Uint32* pixels = surface->pixels;
        int width = surface->w;
        int height = surface->h;
        int h_list[5] = {0,0,0,0,0};
        int w_list[5] = {0,0,0,0,0};
        int cut_h = height / 5;
        int cut_w = width / 5;
        for (int i = 1; i < 5; i++)
        {
                h_list[i] += cut_h * i;
                w_list[i] += cut_w * i;
        }
	SDL_LockSurface(surface);
        for (int k = 0; k < 5; k++)
        {
                binarize_square(pixels, h_list[k], w_list[k], h_list[k+1], w_list[k+1], width, height);
        }
	SDL_UnlockSurface(surface);
	IMG_SavePNG(surface, "out.png");
}


int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    // - Initialize the SDL.
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    
    // - Create a window.
    SDL_Window* window = SDL_CreateWindow("Image", 0, 0, 640, 400,
		    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    
    // - Create a renderer.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
                errx(EXIT_FAILURE, "%s", SDL_GetError());

    // - Create a surface from the colored image.
    SDL_Surface* surface = load_image(argv[1]);

    // - Resize the window according to the size of the image.
    SDL_SetWindowSize(window, surface->w, surface->h);

    // - Convert the surface into grayscale.
    surface_to_grayscale(surface);

    // - Create a new texture from the grayscale surface.
    SDL_Surface* grey = SDL_CreateTextureFromSurface(renderer, surface);

    // - Binarize
    binarize_surface(surface);
    IMG_SavePNG(surface, "out.png");

    // - Free the surface.
    SDL_FreeSurface(surface);

    // - Dispatch the events.
    event_loop(renderer, grey);

    // - Destroy the objects.
    SDL_DestroyTexture(grey);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
