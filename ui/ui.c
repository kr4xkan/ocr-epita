#include <gtk/gtk.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_surface.h>
#include<SDL2/SDL_ttf.h>


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
GtkWidget *image;

//binarisation window
GtkWindow *binarisation;
GtkPaned *paned_container;
GtkWidget *image2;
GtkCheckButton *grayscale_check;

//solved_window
GtkWindow *solved;
GtkFixed *fixed_container3;
GtkWidget *image3;

gchar* filename;

//configure-event (ne marche pas, à implémenter)
gboolean on_configure_opening(GtkWidget *widget, GdkEvent *event, gpointer user_data){
	
	GtkFixed *fixed = user_data;

	gint width = gtk_widget_get_allocated_width(GTK_WIDGET(opening_window));
	gint height = gtk_widget_get_allocated_height(GTK_WIDGET(opening_window));

	//gtk_fixed_move(fixed, fixed->button1, width/5, height/2);
	//gtk_fixed_move(fixed_container, GTK_WIDGET(label1), width/2, 4*height/5);
	//gtk_fixed_move(fixed_container, GTK_WIDGET(text_entry), width/2, height/2);

	return FALSE;
}

//
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

SDL_Surface *LoadImage(const char *path) {
    SDL_Surface *tmp = IMG_Load(path);
    if (tmp == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface *res = SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGB888, 0);
    if (res == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(tmp);
    return res;
}
gchar* select_file(GtkWindow *window, GdkEvent *event, gpointer filename){
	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
	return filename;
}
	

void on_open_button(GtkWidget *widget, GdkEvent *event){
	
	SDL_Surface *surf = LoadImage(filename);
	float ratio = set_gtk_image_from_surface(image, surf, 1);
	SDL_FreeSurface(surf);

	gtk_widget_hide(GTK_WIDGET(opening_window));
	gtk_widget_hide(GTK_WIDGET(solved));
	gtk_widget_hide(GTK_WIDGET(binarisation));
	gtk_widget_show(GTK_WIDGET(manual_rotation));
}


int main(){
	
	gtk_init(NULL, NULL);

	builder = gtk_builder_new_from_file("window_tuto1.glade");
	GError* error = NULL;
	if (gtk_builder_add_from_file(builder, "window_tuto1.glade", &error) == 0){
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
	image = GTK_WIDGET(gtk_builder_get_object(builder,"image1"));

	
	//binarisation components
	binarisation = GTK_WINDOW(gtk_builder_get_object(builder, "binarisation"));
	paned_container = GTK_PANED(gtk_builder_get_object(builder, "paned_container"));
	image = GTK_WIDGET(gtk_builder_get_object(builder, "image2"));


	//solved components
	solved = GTK_WINDOW(gtk_builder_get_object(builder,"solved"));
	fixed_container3 = GTK_FIXED(gtk_builder_get_object(builder, "fixed_container3"));
	image3 = GTK_WIDGET(gtk_builder_get_object(builder, "image3"));


	//signals
	g_signal_connect(opening_window, "destroy", G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(manual_rotation, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(opening_window, "configure-event", G_CALLBACK(on_configure_opening), &fixed_container_1);
	//g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_button), &);
	g_signal_connect(file_chooser, "file-set", G_CALLBACK(select_file), &filename);
	g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_button),&filename);
	//show windows
	gtk_widget_hide(GTK_WIDGET(binarisation));
	gtk_widget_hide(GTK_WIDGET(solved));
	gtk_widget_hide(GTK_WIDGET(manual_rotation));
	gtk_widget_show(GTK_WIDGET(opening_window));
	gtk_main();
	
	return 0;
}
