#pragma once

typedef struct {
    GtkDrawingArea* area;
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

    GtkWindow *opening_window;
    GtkFileChooserButton *file_chooser;
    GtkButton *open_button;


    GtkWindow *line_check_window;
    GtkImage *img_lines;
    GtkButton *next_lines;

    DrawArea draw;
} AppState;
