#include <gtk/gtk.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include "drawarea.h"
#include "ui.h"

GtkBuilder *builder;

//global variables/
GtkWindow *opening_window;
GtkFileChooserButton *file_chooser;
GtkButton *open_button;


GtkWindow *line_check_window;
GtkDrawingArea *area;
GtkButton *next_lines;
GtkImage *img_lines;


SDL_Surface *currentImage;



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

void select_file(GtkWindow *window, GdkEvent *event, gpointer file_chooser){
    currentImage = LoadImage(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser)));
}


void on_open_button(GtkButton *open_button, GdkEvent event, gpointer user_data){
    gtk_widget_show(GTK_WIDGET(line_check_window));
    gtk_widget_show(GTK_WIDGET(next_lines));
    gtk_widget_hide(GTK_WIDGET(opening_window));

    float ratio = set_gtk_image_from_surface(img_lines, currentImage, 1);
    SDL_FreeSurface(currentImage);
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
	opening_window = GTK_WINDOW(gtk_builder_get_object(builder, "opening_window"));
	file_chooser = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "file_chooser"));
	open_button =  GTK_BUTTON(gtk_builder_get_object(builder, "open_button"));

	//Ooening Window:
	line_check_window = GTK_WINDOW(gtk_builder_get_object(builder, "line_check_window"));
    area = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "area"));
	next_lines =  GTK_BUTTON(gtk_builder_get_object(builder, "next_lines"));
    img_lines = GTK_IMAGE(gtk_builder_get_object(builder, "img_lines"));


    AppState app_state = {
        .opening_window = opening_window,
        .file_chooser = file_chooser,
        .open_button = open_button,

        
        .img_lines = img_lines,
        .next_lines = next_lines,
        .next_neural = next_neural,
        .next_solver = next_solver,
        .draw = {
            .area = area,
            .p1 = {0, 0, dotSize, dotSize},
            .p2 = {0, 0, dotSize, dotSize},
            .p3 = {0, 0, dotSize, dotSize},
            .p4 = {0, 0, dotSize, dotSize},
        }
    };

	//signals
	g_signal_connect(opening_window, "destroy", G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_button), NULL);
	g_signal_connect(open_button, "clicked", G_CALLBACK(on_run), NULL);
	g_signal_connect(file_chooser, "file-set", G_CALLBACK(select_file), &file_chooser);

	//show windows
	gtk_widget_show(GTK_WIDGET(opening_window));
    gtk_widget_hide(GTK_WIDGET(line_check_window));
	gtk_main();
	
	return 0;
}
