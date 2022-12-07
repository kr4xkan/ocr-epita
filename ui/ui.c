#include <gtk/gtk.h>
#include "../cutter/crop-manager.h"
#include "../cutter/cutter.h"
#include "../utils.h"

#define dotSize 6

typedef struct Game{
    GtkWindow *window;
    GtkDrawingArea *area; 
    GdkRectangle p1;
    GdkRectangle p2;
    GdkRectangle p3;
    GdkRectangle p4;
    gint moving;
} Game;



// Event handler for the "clicked" signal of the start button.
void on_run(GtkButton *button, gpointer user_data)
{
    Game *game = user_data;


    SDL_Surface *surface = LoadImage("../DataSample/cutter/4.png");

    unsigned int *accumulator = CreateAccumulator(surface);
    Line *lines = DetectLines(accumulator);

    SDL_Surface *surfaceRotated = CheckRotation(surface, accumulator);
    if (surfaceRotated == NULL) {
        size_t vertLen = 0, horiLen = 0;
        lines = FilterLines(accumulator, lines, &vertLen, &horiLen);

        unsigned int *space = CreateSpace(surface, lines);
        Intersection *intersections = FindIntersections(surface, space, vertLen, horiLen);



        size_t ratio = 2;
        game->p1.x = intersections[0].x/ratio;    
        game->p1.y = intersections[0].y/ratio;    

        game->p2.x = intersections[vertLen-1].x/ratio;    
        game->p2.y = intersections[vertLen-1].y/ratio;    

        game->p3.x = intersections[vertLen*horiLen-vertLen].x/ratio;    
        game->p3.y = intersections[vertLen*horiLen-vertLen].y/ratio;    
        
        game->p4.x = intersections[vertLen*horiLen-1].x/ratio;    
        game->p4.y = intersections[vertLen*horiLen-1].y/ratio;    

        //CropSquares(surface, intersections, vertLen, horiLen);
        
        free(space);
        free(intersections);
    }
    else{
        unsigned int *accumulatorRotated = CreateAccumulator(surfaceRotated);
        Line *linesRotated = DetectLines(accumulatorRotated);

        size_t vertLen = 0, horiLen = 0;
        linesRotated = FilterLines(accumulatorRotated, linesRotated, &vertLen, &horiLen);

        unsigned int *spaceRotated = CreateSpace(surfaceRotated, linesRotated);
        Intersection *intersections = FindIntersections(surfaceRotated, spaceRotated, vertLen, horiLen);

        size_t ratio = 2;
        game->p1.x = intersections[0].x/ratio;    
        game->p1.y = intersections[0].y/ratio;    

        game->p2.x = intersections[vertLen-1].x/ratio;    
        game->p2.y = intersections[vertLen-1].y/ratio;    

        game->p3.x = intersections[vertLen*horiLen-vertLen].x/ratio;    
        game->p3.y = intersections[vertLen*horiLen-vertLen].y/ratio;    
        
        game->p4.x = intersections[vertLen*horiLen-1].x/ratio;    
        game->p4.y = intersections[vertLen*horiLen-1].y/ratio;    


        //CropSquares(surfaceRotated, intersections, vertLen, horiLen);

        free(accumulatorRotated);
        free(linesRotated);
        free(spaceRotated);
        free(intersections);
        SDL_FreeSurface(surfaceRotated);
    } 
    free(accumulator);
    free(lines);
    SDL_FreeSurface(surface);

    gtk_widget_queue_draw(GTK_WIDGET(game->area));
}




gint GetDist(int x, int y, Game *game){
    gint dists[4] = {};
    dists[0] = (game->p1.x-x)*(game->p1.x-x) + (game->p1.y-y)*(game->p1.y-y);
    dists[1] = (game->p2.x-x)*(game->p2.x-x) + (game->p2.y-y)*(game->p2.y-y);
    dists[2] = (game->p3.x-x)*(game->p3.x-x) + (game->p3.y-y)*(game->p3.y-y);
    dists[3] = (game->p4.x-x)*(game->p4.x-x) + (game->p4.y-y)*(game->p4.y-y);

    gint min = dists[0];
    gint res = 0;
    for(gint i = 1; i < 4; i++){
        if (dists[i] < min){
            res =  i;
            min = dists[i];
        }
    }

    return min < 1000 ? res+1 : -1;
}



void on_cursor_motion(GtkDrawingArea *area, GdkEventButton *event, gpointer user_data){
    Game *game = user_data;

    gint x, y;
    gdk_window_get_pointer(event->window, &x, &y, NULL);
    if (game->moving == 0){
        return;
    }

    switch (game->moving){
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






void on_press(GtkDrawingArea *area, GdkEventButton *event, gpointer user_data)
{
    Game *game = user_data;

    if (event->button == 1) {
        gint toMove = GetDist(event->x, event->y, game);
        //printf("toMove:%i\n", toMove);
        switch (toMove){
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


void on_release(GtkDrawingArea *area, GdkEventButton *event, gpointer user_data)
{
    Game *game = user_data;

    if (game->moving != 0)
        game->moving = 0;        
}


gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    Game *game = user_data;

    size_t shift = dotSize / 2;;
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_rectangle(cr, (game->p1.x-shift), (game->p1.y-shift), dotSize, dotSize);
    cairo_rectangle(cr, (game->p2.x-shift), (game->p2.y-shift), dotSize, dotSize);
    cairo_rectangle(cr, (game->p3.x-shift), (game->p3.y-shift), dotSize, dotSize);
    cairo_rectangle(cr, (game->p4.x-shift), (game->p4.y-shift), dotSize, dotSize);



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


int main ()
{
    // Initializes GTK.
    gtk_init(NULL, NULL);

    // Constructs a GtkBuilder instance.
    GtkBuilder* builder = gtk_builder_new ();

    // Loads the UI description.
    // (Exits if an error occurs.)
    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "ui.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    // Gets the widgets.
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "test"));
    GtkDrawingArea* area = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "area"));
    GtkButton* run_button = GTK_BUTTON(gtk_builder_get_object(builder, "run_button"));
    //GtkButton* confirm_button = GTK_BUTTON(gtk_builder_get_object(builder, "confirm_button"));


    Game game = {
        .window = window,
        .area = area,
        .p1 = {0, 0, dotSize, dotSize},
        .p2 = {0, 0, dotSize, dotSize},
        .p3 = {0, 0, dotSize, dotSize},
        .p4 = {0, 0, dotSize, dotSize},
    };

    gtk_widget_add_events (GTK_WIDGET(area), GDK_BUTTON_PRESS_MASK);
    gtk_widget_add_events (GTK_WIDGET(area), GDK_BUTTON_RELEASE_MASK);
    gtk_widget_add_events (GTK_WIDGET(area), GDK_POINTER_MOTION_MASK);

    // Connects event handlers.
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(area, "draw", G_CALLBACK(on_draw), &game);
    g_signal_connect(area, "button-press-event", G_CALLBACK(on_press), &game);
    g_signal_connect(area, "button-release-event", G_CALLBACK(on_release), &game);
    g_signal_connect(area, "motion-notify-event", G_CALLBACK(on_cursor_motion), &game);
    g_signal_connect(run_button, "clicked", G_CALLBACK(on_run), &game);
    //g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_confirm), NULL);

    // Runs the main loop.
    gtk_main();

    // Exits.
    return 0;
}
