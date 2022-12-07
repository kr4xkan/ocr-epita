#include <gtk/gtk.h>
#include "../cutter/crop-manager.h"
#include "../cutter/cutter.h"
#include "../utils.h"

#define dotSize 6

typedef struct Feur{
    GtkDrawingArea *area; 
    Intersection p1;
    Intersection p2;
    Intersection p3;
    Intersection p4;
} Feur;



// Event handler for the "clicked" signal of the start button.
void on_run(GtkButton *button, gpointer user_data)
{
    Feur *feur = user_data;


    SDL_Surface *surface = LoadImage("../DataSample/cutter/4.png");

    unsigned int *accumulator = CreateAccumulator(surface);
    Line *lines = DetectLines(accumulator);

    SDL_Surface *surfaceRotated = CheckRotation(surface, accumulator);
    if (surfaceRotated == NULL) {
        size_t vertLen = 0, horiLen = 0;
        lines = FilterLines(accumulator, lines, &vertLen, &horiLen);

        unsigned int *space = CreateSpace(surface, lines);
        Intersection *intersections = FindIntersections(surface, space, vertLen, horiLen);
        
        feur->p1 = intersections[0];    
        feur->p2 = intersections[vertLen-1];    
        feur->p3 = intersections[vertLen*(horiLen-1)];    
        feur->p4 = intersections[vertLen*horiLen-1];    
        
        feur->p1.x /= 10;
        feur->p1.y /= 10;
        
        feur->p2.x /= 10;
        feur->p2.y /= 10;

        feur->p3.x /= 10;
        feur->p3.y /= 10;

        feur->p4.x /= 10;
        feur->p4.y /= 10;


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

        feur->p1 = intersections[0];    
        feur->p2 = intersections[vertLen-1];    
        feur->p3 = intersections[vertLen*horiLen-vertLen];    
        feur->p4 = intersections[vertLen*horiLen-1];    

        feur->p1.x /= 10;
        feur->p1.y /= 10;
        
        feur->p2.x /= 10;
        feur->p2.y /= 10;

        feur->p3.x /= 10;
        feur->p3.x /= 10;

        feur->p4.y /= 10;
        feur->p4.y /= 10;

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
}

/*
// Event handler for the "clicked" signal of the stop button.
void on_confirm(GtkButton *button, gpointer user_data)
{

}
*/


gint GetDist(int x, int y, Feur *feur){
    gint dists[4] = {};
    dists[0] = (feur->p1.x-x)*(feur->p1.x-x) + (feur->p1.y-y)*(feur->p1.y-y);
    dists[1] = (feur->p2.x-x)*(feur->p2.x-x) + (feur->p2.y-y)*(feur->p2.y-y);
    dists[2] = (feur->p3.x-x)*(feur->p3.x-x) + (feur->p3.y-y)*(feur->p3.y-y);
    dists[3] = (feur->p4.x-x)*(feur->p4.x-x) + (feur->p4.y-y)*(feur->p4.y-y);
    printf("dists[0]:%i\n", dists[0]);
    printf("dists[1]:%i\n", dists[1]);
    printf("dists[2]:%i\n", dists[2]);
    printf("dists[3]:%i\n", dists[3]);

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

gboolean on_press(GtkDrawingArea *area, GdkEventButton *event, gpointer user_data)
{
    Feur *feur = user_data;

    if (event->button == 1) {
        gint toMove = GetDist(event->x, event->y, feur);
        printf("toMove:%i\n", toMove);
        switch (toMove){
            case 1:
                feur->p1.x = event->x;
                feur->p1.y = event->y;
                break;
            case 2:
                feur->p2.x = event->x;
                feur->p2.y = event->y;
                break;
            case 3:
                feur->p3.x = event->x;
                feur->p3.y = event->y;
                break;
            case 4:
                feur->p4.x = event->x;
                feur->p4.y = event->y;
                break;
        }
        gtk_widget_queue_draw(GTK_WIDGET(area));
    }

    return TRUE;
}


gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    Feur *feur = user_data;

    size_t shift = dotSize / 2;;
    cairo_set_source_rgb(cr, 0, 1, 0);
    cairo_rectangle(cr, (feur->p1.x-shift), (feur->p1.y-shift), dotSize, dotSize);
    cairo_rectangle(cr, (feur->p2.x-shift), (feur->p2.y-shift), dotSize, dotSize);
    cairo_rectangle(cr, (feur->p3.x-shift), (feur->p3.y-shift), dotSize, dotSize);
    cairo_rectangle(cr, (feur->p4.x-shift), (feur->p4.y-shift), dotSize, dotSize);
    cairo_fill(cr);

    return FALSE;
}


int main (int argc, char *argv[])
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
    GtkButton* confirm_button = GTK_BUTTON(gtk_builder_get_object(builder, "confirm_button"));


    Feur feur = {
        .area = area,
        .p1 = {0, 0},
        .p2 = {0, 0},
        .p3 = {0, 0},
        .p4 = {0, 0},
    };

    gtk_widget_add_events (GTK_WIDGET(area), GDK_BUTTON_PRESS_MASK);
    gtk_widget_add_events (GTK_WIDGET(area), GDK_BUTTON_RELEASE_MASK);

    // Connects event handlers.
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(area, "draw", G_CALLBACK(on_draw), &feur);
    g_signal_connect(area, "button-press-event", G_CALLBACK(on_press), &feur);
    g_signal_connect(area, "button-release-event", G_CALLBACK(on_press), &feur);
    g_signal_connect(run_button, "clicked", G_CALLBACK(on_run), &feur);
    //g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_confirm), NULL);

    // Runs the main loop.
    gtk_main();

    // Exits.
    return 0;
}
