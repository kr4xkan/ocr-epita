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
    GtkWindow* w_home;
    GtkWindow* w_preprocessing;
    GtkWindow* w_lines;
    GtkWindow* w_neural;
    GtkWindow* w_solver;
    GtkImage* img_preprocessing;
    GtkImage* img_lines;
    GtkImage* img_neural;
    GtkImage* img_solver;
    GtkButton* next_preprocessing;
    GtkButton* next_lines;
    GtkButton* next_neural;
    GtkButton* next_solver;
    gchar* current_image;
    DrawArea draw;
} AppState;
