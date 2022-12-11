#pragma once

#include "../cutter/crop-manager.h"
#include "../cutter/cutter.h"
#include "../utils.h"
#include "types.h"
#include "ui.h"
#include <gtk/gtk.h>

#define dotSize 6

void Run(AppState *state);
gint GetDist(int x, int y, DrawArea *game);
void on_cursor_motion(GtkDrawingArea *area, GdkEventButton *event,
                      gpointer user_data);
void on_press(GtkDrawingArea *area, GdkEventButton *event, gpointer user_data);
void on_release(GtkDrawingArea *area, GdkEventButton *event,
                gpointer user_data);
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data);
