#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "../cutter/cutter.h"
#include "../neural-net/neural-net.h"
#include "../preprocessing/preprocessing.h"
#include "../solver/solver.h"
#include "../utils.h"


//global variables
GtkBuilder *builder;

//opening window
GtkWindow *opening_window;
GtkWidget *fixed_container_1;
GtkButton *open_button;
GtkLabel *label1;
GtkLabel *label1;
GtkFileChooserButton *file_chooser;

//manual rotation_window
GtkWindow *manual_rotation;
GtkImage *image;
SDL_Surface *surf_rot;
SDL_Surface *tmp;
GtkButton *next_button_rot;
GtkEntry *angle_entry;

//binarisation window
GtkWindow *binarisation;
GtkFixed *fixed_container_bin;
SDL_Surface *surf_bin;
GtkImage *bin_image;
GtkToggleButton *binarize_check;
GtkButton *next_button_bin;
GtkButton *next_button_cutter;

//solved_window
GtkWindow *solved;
GtkFixed *fixed_container3;
GtkImage *image3;
GtkButton *save_button;

gchar* filename;

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

SDL_Surface *loadImage(const char *path) {
    SDL_Surface *tmp = IMG_Load(path);
    if (tmp == NULL)
	errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Surface *res = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGB888, 0);
    if (res == NULL)
	errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_FreeSurface(tmp);
    return res;
}
gchar* select_file(GtkWindow *window, GdkEvent *event){
	surf_rot = loadImage(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser)));
	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
	return filename;
}
	














void on_open_button(GtkWidget *widget, GdkEvent *event){
	set_gtk_image_from_surface(image, surf_rot, 1);

	gtk_widget_hide(GTK_WIDGET(opening_window));
	gtk_widget_show(GTK_WIDGET(manual_rotation));
}




void on_rotate(GtkWidget *widget, GdkEvent *event){

	tmp = surf_rot;
	const gchar* angle_text;
	angle_text = gtk_entry_get_text(angle_entry);

	if (angle_text == NULL){
		tmp = surf_rot;
	}
	else{

		float angle =  atof(angle_text);
		tmp = RotateSurface(surf_rot, angle);
	}
	set_gtk_image_from_surface(image, tmp, 1);

}
void on_next_button_rot(GtkWidget *widget, GdkEvent *event){

//binarisation
	surf_rot = tmp;
	binarization(surf_rot);
	surf_bin = loadImage("binary.png");
	set_gtk_image_from_surface(bin_image, surf_bin, 1);

	gtk_widget_hide(GTK_WIDGET(manual_rotation));
	gtk_toggle_button_set_active(binarize_check, TRUE);
	gtk_widget_show(GTK_WIDGET(binarisation));
}

void on_binarize_check(GtkWidget *widget, GdkEvent *event){
	gboolean checked = gtk_toggle_button_get_active(binarize_check);
	if(checked){
		set_gtk_image_from_surface(bin_image, surf_bin,1);
	}
	else{
		set_gtk_image_from_surface(bin_image, surf_rot,1);
	}
}


void on_next_button_bin(GtkWidget *widget, GdkEvent *event){
// partie de romain
	gtk_widget_hide(GTK_WIDGET(binarisation));
	gtk_widget_show(GTK_WIDGET(solved));
}


void on_next_button_cutter( GtkWidget *widget, GdkEvent *event){}

void on_save_button(GtkWidget *widget, GdkEvent *event){
}

















int main(){
	
	gtk_init(NULL, NULL);

	builder = gtk_builder_new_from_file("window_tuto1.glade");
	GError* error = NULL;
	if (gtk_builder_add_from_file(builder, "window_ui.glade", &error) == 0){
		g_printerr("Error loading file: %s\n", error->message);
		g_clear_error(&error);
		return 1;
	}


	//get components
	//opening window components
	opening_window = GTK_WINDOW(gtk_builder_get_object(builder, "opening_window"));
	fixed_container_1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed_container_1"));
	label1 = GTK_LABEL(gtk_builder_get_object(builder, "label1"));
	open_button =  GTK_BUTTON(gtk_builder_get_object(builder, "open_button"));
	
	//manual rotation components
	manual_rotation = GTK_WINDOW(gtk_builder_get_object(builder, "manual_rotation"));
	file_chooser = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "file_chooser"));
	image = GTK_IMAGE(gtk_builder_get_object(builder,"image1"));
	next_button_rot = GTK_BUTTON(gtk_builder_get_object(builder,"next_button_rot"));
	angle_entry = GTK_ENTRY(gtk_builder_get_object(builder, "angle_entry"));
	
	//binarisation components
	binarisation = GTK_WINDOW(gtk_builder_get_object(builder, "binarisation"));
	fixed_container_bin = GTK_FIXED(gtk_builder_get_object(builder, "fixed_container_bin"));
	bin_image = GTK_IMAGE(gtk_builder_get_object(builder, "bin_image"));
	binarize_check = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "binarize_check"));
	next_button_bin = GTK_BUTTON(gtk_builder_get_object(builder,"next_button_bin"));

	next_button_cutter = GTK_BUTTON(gtk_builder_get_object(builder,"next_button_cutter"));

	//solved components
	solved = GTK_WINDOW(gtk_builder_get_object(builder,"solved"));
	fixed_container3 = GTK_FIXED(gtk_builder_get_object(builder, "fixed_container3"));
	image3 = GTK_IMAGE(gtk_builder_get_object(builder, "image3"));
	save_button = GTK_BUTTON(gtk_builder_get_object(builder, "save_button"));


	//signals
	g_signal_connect(opening_window, "destroy", G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(manual_rotation, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(binarisation, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(solved, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	
	g_signal_connect(file_chooser, "file-set", G_CALLBACK(select_file),NULL);
	g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_button),NULL);
	g_signal_connect(next_button_rot, "clicked", G_CALLBACK(on_next_button_rot), NULL);
	g_signal_connect(angle_entry, "changed", G_CALLBACK(on_rotate), NULL);
	g_signal_connect(binarize_check, "toggled", G_CALLBACK(on_binarize_check),NULL);
	g_signal_connect(next_button_bin, "clicked", G_CALLBACK(on_next_button_bin), NULL);
	g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button), NULL);

	//show windows
	gtk_widget_hide(GTK_WIDGET(binarisation));
	gtk_widget_hide(GTK_WIDGET(solved));
	gtk_widget_hide(GTK_WIDGET(manual_rotation));
	gtk_widget_show(GTK_WIDGET(opening_window));
	gtk_main();
	
	return 0;
}
