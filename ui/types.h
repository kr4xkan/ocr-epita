#pragma once
#include <SDL2/SDL.h>
#include <gtk/gtk.h>

typedef struct {
    GtkDrawingArea *area;
    GdkRectangle p1;
    GdkRectangle p2;
    GdkRectangle p3;
    GdkRectangle p4;
    gint moving;
    double ratio;
} DrawArea;

typedef struct {
    gchar *current_image;
    SDL_Surface *current_surface;
    SDL_Surface **cells;

    GtkWindow *line_check_window;
    GtkImage *img_lines;
    GtkButton *next_lines;

    int sudoku[9][9];

    DrawArea draw;
} AppState;
