#include "drawarea.h"

void Run(AppState *state) {
    DrawArea *game = &state->draw;

    SDL_Surface *surface = state->current_surface;
    unsigned int *accumulator = CreateAccumulator(surface);

    Line *lines = DetectLines(accumulator);

    int angle = 0;
    SDL_Surface *surfaceRotated = CheckRotation(surface, accumulator, &angle);
    double ratio = game->ratio;
    if (surfaceRotated == NULL) {
        size_t vertLen = 0, horiLen = 0;
        lines = FilterLines(accumulator, lines, &vertLen, &horiLen);

        unsigned int *space = CreateSpace(surface, lines);
        Intersection *intersections =
            FindIntersections(surface, space, vertLen, horiLen);

        game->p1.x = intersections[0].x * ratio;
        game->p1.y = intersections[0].y * ratio;

        game->p2.x = intersections[vertLen - 1].x * ratio;
        game->p2.y = intersections[vertLen - 1].y * ratio;

        game->p3.x = intersections[vertLen * horiLen - vertLen].x * ratio;
        game->p3.y = intersections[vertLen * horiLen - vertLen].y * ratio;

        game->p4.x = intersections[vertLen * horiLen - 1].x * ratio;
        game->p4.y = intersections[vertLen * horiLen - 1].y * ratio;

        // CropSquares(surface, intersections, vertLen, horiLen);

        free(space);
        free(intersections);
    } else {
        state->current_surface = surfaceRotated;
        set_gtk_image_from_surface(state->img_lines, state->current_surface, 1);
        unsigned int *accumulatorRotated = CreateAccumulator(surfaceRotated);
        Line *linesRotated = DetectLines(accumulatorRotated);

        size_t vertLen = 0, horiLen = 0;
        linesRotated =
            FilterLines(accumulatorRotated, linesRotated, &vertLen, &horiLen);

        unsigned int *spaceRotated = CreateSpace(surfaceRotated, linesRotated);
        Intersection *intersections =
            FindIntersections(surfaceRotated, spaceRotated, vertLen, horiLen);

        ratio = (float)580 / (float)surfaceRotated->w;

        game->p1.x = intersections[0].x * ratio;
        game->p1.y = intersections[0].y * ratio;

        game->p2.x = intersections[vertLen - 1].x * ratio;
        game->p2.y = intersections[vertLen - 1].y * ratio;

        game->p3.x = intersections[vertLen * horiLen - vertLen].x * ratio;
        game->p3.y = intersections[vertLen * horiLen - vertLen].y * ratio;

        game->p4.x = intersections[vertLen * horiLen - 1].x * ratio;
        game->p4.y = intersections[vertLen * horiLen - 1].y * ratio;

        // CropSquares(surfaceRotated, intersections, vertLen, horiLen);

        free(accumulatorRotated);
        free(linesRotated);
        free(spaceRotated);
        free(intersections);
    }
    free(accumulator);
    free(lines);

    gtk_widget_queue_draw(GTK_WIDGET(game->area));
}

gint GetDist(int x, int y, DrawArea *game) {
    gint dists[4] = {};
    dists[0] = (game->p1.x - x) * (game->p1.x - x) +
               (game->p1.y - y) * (game->p1.y - y);
    dists[1] = (game->p2.x - x) * (game->p2.x - x) +
               (game->p2.y - y) * (game->p2.y - y);
    dists[2] = (game->p3.x - x) * (game->p3.x - x) +
               (game->p3.y - y) * (game->p3.y - y);
    dists[3] = (game->p4.x - x) * (game->p4.x - x) +
               (game->p4.y - y) * (game->p4.y - y);

    gint min = dists[0];
    gint res = 0;
    for (gint i = 1; i < 4; i++) {
        if (dists[i] < min) {
            res = i;
            min = dists[i];
        }
    }

    return min < 1000 ? res + 1 : -1;
}

void on_cursor_motion(GtkDrawingArea *area, GdkEventButton *event,
                      gpointer user_data) {
    AppState *state = user_data;
    DrawArea *game = &state->draw;

    gint x, y;
    gdk_window_get_pointer(event->window, &x, &y, NULL);
    if (game->moving == 0) {
        return;
    }

    switch (game->moving) {
    case 1:
        game->p1.x = x;
        game->p1.y = y;

        break;
    case 2:
        game->p2.x = x;
        game->p2.y = y;
        break;
    case 3:
        game->p3.x = x;
        game->p3.y = y;
        break;
    default:
        game->p4.x = x;
        game->p4.y = y;
        break;
    }
    gtk_widget_queue_draw(GTK_WIDGET(area));
    return;
}

void on_press(GtkDrawingArea *area, GdkEventButton *event, gpointer user_data) {
    AppState *state = user_data;
    DrawArea *game = &state->draw;

    if (event->button == 1) {
        gint toMove = GetDist(event->x, event->y, game);
        // printf("toMove:%i\n", toMove);
        switch (toMove) {
        case 1:
            game->moving = 1;
            break;
        case 2:
            game->moving = 2;
            break;
        case 3:
            game->moving = 3;
            break;
        case 4:
            game->moving = 4;
            break;
        default:
            game->moving = 0;
            break;
        }
    }

    return;
}

void on_release(GtkDrawingArea *area, GdkEventButton *event,
                gpointer user_data) {
    AppState *state = user_data;
    DrawArea *game = &state->draw;

    if (game->moving != 0)
        game->moving = 0;
}

gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    AppState *state = user_data;
    DrawArea *game = &state->draw;

    size_t shift = dotSize / 2;
    ;
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_rectangle(cr, (game->p1.x - shift), (game->p1.y - shift), dotSize,
                    dotSize);
    cairo_rectangle(cr, (game->p2.x - shift), (game->p2.y - shift), dotSize,
                    dotSize);
    cairo_rectangle(cr, (game->p3.x - shift), (game->p3.y - shift), dotSize,
                    dotSize);
    cairo_rectangle(cr, (game->p4.x - shift), (game->p4.y - shift), dotSize,
                    dotSize);

    cairo_set_line_width(cr, 2);
    cairo_move_to(cr, game->p1.x, game->p1.y);
    cairo_line_to(cr, game->p2.x, game->p2.y);

    cairo_move_to(cr, game->p2.x, game->p2.y);
    cairo_line_to(cr, game->p4.x, game->p4.y);

    cairo_move_to(cr, game->p4.x, game->p4.y);
    cairo_line_to(cr, game->p3.x, game->p3.y);

    cairo_move_to(cr, game->p3.x, game->p3.y);
    cairo_line_to(cr, game->p1.x, game->p1.y);

    cairo_stroke(cr);
    cairo_fill(cr);

    return FALSE;
}
