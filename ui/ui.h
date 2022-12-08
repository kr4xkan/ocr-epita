#pragma once

#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include "drawarea.h"
#include "types.h"
#include "../neural-net/neural-net.h"

double set_gtk_image_from_surface (GtkImage* img_container, SDL_Surface *surface, char preserve_ratio);
SDL_Surface* make_sudoku_grid(char grid[9][9], char changed[9][9]);


