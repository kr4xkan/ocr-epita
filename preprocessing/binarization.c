#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void binarize_surface(SDL_Surface* surface)
{
	Uint32* pixels = surface->pixels;
	int width = surface->w;
	int height = surface->h;
	int h_list[5] = {0,0,0,0,0};
	int w_list[5] = {0,0,0,0,0};
	int cut_h = height / 5;
	int cut_w = width / 5;
	for (int i = 1, i < 5, i++)
	{
		h_list[i] += cut_h * i;
		w_list[i] += cut_w * i;
	}
	for (int k = 0; k < 5; k++)
	{
		Uint32 mid = calcul_mid(pixels, h_list[k], w_list[k], h_list[k+1], w_list[k+1]);
	}
}

Uint32 calcul_mid(Uint32* pixels, int Hmin, int Wmin, Hmax, int Wmax)
{
	
}


void binarize_square(Uint32* pixels, int Hmin, int Wmin, Hmax, int Wmax);
