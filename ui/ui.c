#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>

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
} AppState;

void set_gtk_image_from_surface (GtkImage* img_container, SDL_Surface *surface)
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

    GdkPixbuf* pxbscaled = gdk_pixbuf_scale_simple(pixbuf, 580, 400, GDK_INTERP_BILINEAR);
    
    gtk_image_set_from_pixbuf(img_container, pxbscaled);

    g_object_unref (pixbuf);
    g_object_unref (pxbscaled);
}

void on_select_file(GtkFileChooserButton* self, gpointer user_data) {
    AppState* app_state = user_data;
    app_state->current_image = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(self));

// BINARISATION
// BINARISATION
// BINARISATION
// BINARISATION
// BINARISATION
// BINARISATION
// BINARISATION

    SDL_Surface *surf = IMG_Load(app_state->current_image);
    set_gtk_image_from_surface(app_state->img_preprocessing, surf);
    SDL_FreeSurface(surf);

    gtk_widget_show(GTK_WIDGET(app_state->w_preprocessing));
    gtk_widget_show(GTK_WIDGET(app_state->next_preprocessing));
    gtk_widget_hide(GTK_WIDGET(app_state->w_home));
}

void on_next_preprocessing(GtkButton* self, gpointer user_data) {
    AppState* app_state = user_data;
    gtk_widget_show(GTK_WIDGET(app_state->w_lines));
    gtk_widget_show(GTK_WIDGET(app_state->next_lines));
    gtk_widget_hide(GTK_WIDGET(app_state->w_preprocessing));

    SDL_Surface *surf = IMG_Load(app_state->current_image);
    set_gtk_image_from_surface(app_state->img_lines, surf);
    SDL_FreeSurface(surf);
// DETECT LINES AND CROP
// DETECT LINES AND CROP
// DETECT LINES AND CROP
// DETECT LINES AND CROP
// DETECT LINES AND CROP
// DETECT LINES AND CROP
// DETECT LINES AND CROP
}

void on_next_lines(GtkButton* self, gpointer user_data) {
    AppState* app_state = user_data;
    gtk_widget_show(GTK_WIDGET(app_state->w_neural));
    gtk_widget_show(GTK_WIDGET(app_state->next_neural));
    gtk_widget_hide(GTK_WIDGET(app_state->w_lines));

    SDL_Surface *surf = IMG_Load(app_state->current_image);
    set_gtk_image_from_surface(app_state->img_neural, surf);
    SDL_FreeSurface(surf);
// APPLY NEURAL NETWORKS ON ALL CROPPED CELLS
// APPLY NEURAL NETWORKS ON ALL CROPPED CELLS
// APPLY NEURAL NETWORKS ON ALL CROPPED CELLS
// APPLY NEURAL NETWORKS ON ALL CROPPED CELLS
// APPLY NEURAL NETWORKS ON ALL CROPPED CELLS
// APPLY NEURAL NETWORKS ON ALL CROPPED CELLS
// APPLY NEURAL NETWORKS ON ALL CROPPED CELLS
}

void on_next_neural(GtkButton* self, gpointer user_data) {
    AppState* app_state = user_data;
    gtk_widget_show(GTK_WIDGET(app_state->w_solver));
    gtk_widget_show(GTK_WIDGET(app_state->next_solver));
    gtk_widget_hide(GTK_WIDGET(app_state->w_neural));

    SDL_Surface *surf = IMG_Load(app_state->current_image);
    set_gtk_image_from_surface(app_state->img_solver, surf);
    SDL_FreeSurface(surf);
// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
}

void on_next_solver(GtkButton* self, gpointer user_data) {
    AppState* app_state = user_data;
    gtk_widget_show(GTK_WIDGET(app_state->w_home));
    gtk_widget_hide(GTK_WIDGET(app_state->w_solver));
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
    if (gtk_builder_add_from_file(builder, "main.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    // Gets the widgets.
    GtkWindow* w_home = GTK_WINDOW(
            gtk_builder_get_object(builder, "org.pc2.ocr"));
    GtkWindow* w_preprocessing = GTK_WINDOW(
            gtk_builder_get_object(builder, "w_preprocessing"));
    GtkWindow* w_lines = GTK_WINDOW(
            gtk_builder_get_object(builder, "w_lines"));
    GtkWindow* w_neural = GTK_WINDOW(
            gtk_builder_get_object(builder, "w_neural"));
    GtkWindow* w_solver = GTK_WINDOW(
            gtk_builder_get_object(builder, "w_solver"));

    GtkImage* img_preprocessing = GTK_IMAGE(
            gtk_builder_get_object(builder, "img_preprocessing"));
    GtkImage* img_lines = GTK_IMAGE(
            gtk_builder_get_object(builder, "img_lines"));
    GtkImage* img_neural = GTK_IMAGE(
            gtk_builder_get_object(builder, "img_neural"));
    GtkImage* img_solver = GTK_IMAGE(
            gtk_builder_get_object(builder, "img_solver"));

    GtkFileChooserButton* file_chooser = GTK_FILE_CHOOSER_BUTTON(
            gtk_builder_get_object(builder, "file_chooser"));
    GtkButton* next_preprocessing = GTK_BUTTON(
            gtk_builder_get_object(builder, "next_preprocessing"));
    GtkButton* next_lines = GTK_BUTTON(
            gtk_builder_get_object(builder, "next_lines"));
    GtkButton* next_neural = GTK_BUTTON(
            gtk_builder_get_object(builder, "next_neural"));
    GtkButton* next_solver = GTK_BUTTON(
            gtk_builder_get_object(builder, "next_solver"));

    AppState app_state = {
        .w_home = w_home,
        .w_preprocessing = w_preprocessing,
        .w_lines = w_lines,
        .w_neural = w_neural,
        .w_solver = w_solver,
        .img_preprocessing = img_preprocessing,
        .img_lines = img_lines,
        .img_neural = img_neural,
        .img_solver = img_solver,
        .next_preprocessing = next_preprocessing,
        .next_lines = next_lines,
        .next_neural = next_neural,
        .next_solver = next_solver,
    };

    // Connects event handlers.
    g_signal_connect(file_chooser, "file-set", G_CALLBACK(on_select_file), &app_state);
    g_signal_connect(next_preprocessing, "clicked", G_CALLBACK(on_next_preprocessing), &app_state);
    g_signal_connect(next_lines, "clicked", G_CALLBACK(on_next_lines), &app_state);
    g_signal_connect(next_neural, "clicked", G_CALLBACK(on_next_neural), &app_state);
    g_signal_connect(next_solver, "clicked", G_CALLBACK(on_next_solver), &app_state);
    g_signal_connect(w_home, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(w_preprocessing, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(w_lines, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(w_neural, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(w_solver, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show(GTK_WIDGET(w_home));
    // Runs the main loop.
    gtk_main();

    // Exits.
    return 0;
}
