#pragma once
#include <SDL2/SDL.h>
#include <gtk/gtk.h>
double set_gtk_image_from_surface(GtkImage *img_container, SDL_Surface *surface,
                                  char preserve_ratio);
int main_ui();
