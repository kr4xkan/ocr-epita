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
    SDL_Surface *currentImage;

    GtkWindow *opening_window;
    GtkFileChooserButton *file_chooser;
    GtkButton *open_button;


    GtkWindow *line_check_window;
    GtkDrawingArea *area;
    GtkButton *next_lines;
    GtkImage *img_lines;

    DrawArea draw;
} AppState;
