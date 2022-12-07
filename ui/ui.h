#pragma once

#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include "drawarea.h"
#include "types.h"

SDL_Surface* make_sudoku_grid(char grid[9][9], char changed[9][9]);
