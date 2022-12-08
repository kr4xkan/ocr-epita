#include <gtk/gtk.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>

#include "drawarea.h"
#include "ui.h"

GtkBuilder *builder;




double set_gtk_image_from_surface (GtkImage* img_container, SDL_Surface *surface, char preserve_ratio)
{
    Uint32 src_format;
    Uint32 dst_format;

    GdkPixbuf *pixbuf;
    gboolean has_alpha;
    int rowstride;
    guchar *pixels;

    // select format                                                            
    src_format = surface->format->format;
    has_alpha = SDL_ISPIXELFORMAT_ALPHA(src_format);
    if (has_alpha) {
        dst_format = SDL_PIXELFORMAT_RGBA32;
    }
    else {
        dst_format = SDL_PIXELFORMAT_RGB24;
    }

    // create pixbuf                                                 
    pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, has_alpha, 8,
            surface->w, surface->h);
    rowstride = gdk_pixbuf_get_rowstride (pixbuf);
    pixels = gdk_pixbuf_get_pixels (pixbuf);


    // copy pixels                                                              
    SDL_LockSurface(surface);
    SDL_ConvertPixels (surface->w, surface->h, src_format,
            surface->pixels, surface->pitch,
            dst_format, pixels, rowstride);
    SDL_UnlockSurface(surface);

    int nW = 580;
    int nH;
    if (preserve_ratio)
        nH = surface->h * nW / surface->w;
    else
        nH = 400;

    GdkPixbuf* pxbscaled = gdk_pixbuf_scale_simple(pixbuf, nW, nH, GDK_INTERP_BILINEAR);

    gtk_image_set_from_pixbuf(img_container, pxbscaled);

    g_object_unref (pixbuf);
    g_object_unref (pxbscaled);
    return (double)nW/(double)surface->w;
}

void select_file(GtkWindow *window, gpointer file_chooser){
    AppState *app_state = file_chooser;
    app_state->current_image = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(app_state->file_chooser));
}


void on_open_button(GtkButton *open_button, gpointer user_data){
    AppState *app_state = user_data;

    gtk_widget_show(GTK_WIDGET(app_state->line_check_window));
    gtk_widget_show(GTK_WIDGET(app_state->next_lines));
    gtk_widget_hide(GTK_WIDGET(app_state->opening_window));

    app_state->current_surface = IMG_Load(app_state->current_image);
    app_state->draw.ratio = set_gtk_image_from_surface(app_state->img_lines, app_state->current_surface, 1);
}

void on_run(GtkButton *open_button, gpointer user_data){
    AppState *app_state = user_data;
    Run(app_state);
}




void on_next_lines(GtkButton* button, gpointer user_data) {
    AppState* app_state = user_data;
    //SHOW NEXT WINDOW AND HIDE CURRENT AND BUTTON

    Intersection* corners = calloc(4, sizeof(Intersection));
    printf("(%u, %u)\n", app_state->draw.p1.x, app_state->draw.p1.y);
    printf("(%u, %u)\n", app_state->draw.p2.x, app_state->draw.p2.y);
    printf("(%u, %u)\n", app_state->draw.p3.x, app_state->draw.p3.y);
    printf("(%u, %u)\n", app_state->draw.p4.x, app_state->draw.p4.y);
        
    corners[0].x = app_state->draw.p1.x/app_state->draw.ratio;
    corners[0].y = app_state->draw.p1.y/app_state->draw.ratio;
    corners[1].x = app_state->draw.p2.x/app_state->draw.ratio;
    corners[1].y = app_state->draw.p2.y/app_state->draw.ratio;
    corners[2].x = app_state->draw.p3.x/app_state->draw.ratio;
    corners[2].y = app_state->draw.p3.y/app_state->draw.ratio;
    corners[3].x = app_state->draw.p4.x/app_state->draw.ratio;
    corners[3].y = app_state->draw.p4.y/app_state->draw.ratio;

    app_state->cells = ManualCutter(app_state->current_surface, corners);
    printf("all cells have been cropped\n");

    // APPLY NEURAL NETWORKS ON ALL CROPPED CELLS
    // APPLY NEURAL NETWORKS ON ALL CROPPED CELLS
    // APPLY NEURAL NETWORKS ON ALL CROPPED CELLS
}


int main(){

    gtk_init(NULL, NULL);

    builder = gtk_builder_new_from_file("main.glade");
    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "main.glade", &error) == 0){
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }


    //Ooening Window:
    GtkWindow *opening_window = GTK_WINDOW(gtk_builder_get_object(builder, "opening_window"));
    GtkFileChooserButton *file_chooser = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "file_chooser"));
    GtkButton *open_button =  GTK_BUTTON(gtk_builder_get_object(builder, "open_button"));

    //Line Check
    GtkWindow *line_check_window = GTK_WINDOW(gtk_builder_get_object(builder, "line_check_window"));
    GtkDrawingArea *area = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "area"));
    GtkImage *img_lines = GTK_IMAGE(gtk_builder_get_object(builder, "img_lines"));
    GtkButton *next_lines =  GTK_BUTTON(gtk_builder_get_object(builder, "next_lines"));

    AppState app_state = {
        .opening_window = opening_window,
        .file_chooser = file_chooser,
        .open_button = open_button,

        .line_check_window = line_check_window,
        .img_lines = img_lines,
        .next_lines = next_lines,
        .draw = {
            .area = area,
            .p1 = {0, 0, dotSize, dotSize},
            .p2 = {0, 0, dotSize, dotSize},
            .p3 = {0, 0, dotSize, dotSize},
            .p4 = {0, 0, dotSize, dotSize},
        }
    };



    //First Window
    g_signal_connect(opening_window, "destroy", G_CALLBACK(gtk_main_quit),NULL);
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_button), &app_state);
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_run), &app_state);
    g_signal_connect(file_chooser, "file-set", G_CALLBACK(select_file), &app_state);


    //Second Window
    gtk_widget_add_events (GTK_WIDGET(area), GDK_BUTTON_PRESS_MASK);
    gtk_widget_add_events (GTK_WIDGET(area), GDK_BUTTON_RELEASE_MASK);
    gtk_widget_add_events (GTK_WIDGET(area), GDK_POINTER_MOTION_MASK);
    g_signal_connect(area, "draw", G_CALLBACK(on_draw), &app_state);
    g_signal_connect(area, "button-press-event", G_CALLBACK(on_press), &app_state);
    g_signal_connect(area, "button-release-event", G_CALLBACK(on_release), &app_state);
    g_signal_connect(area, "motion-notify-event", G_CALLBACK(on_cursor_motion), &app_state);

    g_signal_connect(next_lines, "clicked", G_CALLBACK(on_next_lines), &app_state);

    //show windows
    gtk_widget_show(GTK_WIDGET(opening_window));
    gtk_widget_hide(GTK_WIDGET(line_check_window));
    gtk_main();

    return 0;
}
