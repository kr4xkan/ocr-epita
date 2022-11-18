#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>

#define PADDLE_STEP 5               // Step of a paddle in pixels
#define PADDLE_PERIOD 5             // Period of a paddle in milliseconds
#define DISC_PERIOD 4               // Period of the disc in milliseconds
#define END_GAME_SCORE 5            // Maximum number of points for a player

// State of the game.
typedef enum State
{
    STOP,                           // Stop state
    PLAY,                           // Play state
    PAUSE,                          // Pause state
} State;

// Structure of a player.
typedef struct Player
{
    GdkRectangle rect;              // Position and size of the player's paddle
    gint step;                      // Vertical step of the player's paddle in pixels
    guint score;                    // Score
    GtkLabel* label;                // Label used to display the score
    guint event;                    // Event ID used to move the paddle
} Player;

// Structure of the disc.
typedef struct Disc
{
    GdkRectangle rect;              // Position and size
    GdkPoint step;                  // Horizontal and verical steps in pixels
    guint period;                   // Period in milliseconds
    guint event;                    // Event ID used to move the disc
} Disc;

// Structure of the graphical user interface.
typedef struct UserInterface
{
    GtkWindow* window;              // Main window
    GtkDrawingArea* area;           // Drawing area
    GtkButton* start_button;        // Start button
    GtkButton* stop_button;         // Stop button
    GtkScale* speed_scale;          // Speed scale
    GtkCheckButton* training_cb;    // Training check box
} UserInterface;

// Structure of the game.
typedef struct Game
{
    State state;                    // State of the game
    Player p1;                      // Player 1
    Player p2;                      // Player 2
    Disc disc;                      // Disc
    UserInterface ui;               // User interface
} Game;

// Event handler for the "draw" signal of the drawing area.
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    // Gets the 'Game' structure.
    Game *game = user_data;

    // Sets the background to white.
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    // Draws the disc in red.
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_rectangle(cr, game->disc.rect.x, game->disc.rect.y,
        game->disc.rect.width, game->disc.rect.height);
    cairo_fill(cr);

    // Propagates the signal.
    return FALSE;
}

// Redraws an item in the drawing area.
//
// Parameters:
//
// * `area`: Drawing area.
// * `old`: Previous position of the item to redraw.
// * `new`: New position of the item to redraw.
void redraw_item(GtkDrawingArea *area, GdkRectangle *old, GdkRectangle *new)
{
    gdk_rectangle_union(old, new, old);

    gtk_widget_queue_draw_area(GTK_WIDGET(area), old->x, old->y, old->width, old->height);
}

// Timeout function called at regular intervals to move the disc.
gboolean on_move_disc(gpointer user_data)
{
    // Gets the `Game` structure passed as parameter.
    Game* game = user_data;

    // Gets the largest coordinate for the disc.
    gint x_max = gtk_widget_get_allocated_width(GTK_WIDGET(game->ui.area))
        - game->disc.rect.width;
    gint y_max = gtk_widget_get_allocated_height(GTK_WIDGET(game->ui.area))
        - game->disc.rect.height;

    // Gets the current position of the disc.
    GdkRectangle old = game->disc.rect;

    if (game->disc.rect.x >= x_max || game->disc.rect.x <= 0) {
        game->disc.step.x *= -1;
    }
    if (game->disc.rect.y >= y_max || game->disc.rect.y <= 0) {
        game->disc.step.y *= -1;
    }

    // Works out the new position of the disc.
    game->disc.rect.x = CLAMP(game->disc.rect.x + game->disc.step.x, 0, x_max);
    game->disc.rect.y = CLAMP(game->disc.rect.y + game->disc.step.y, 0, y_max);

    // Redraws the disc.
    redraw_item(game->ui.area, &old, &game->disc.rect);

    // Enables the next call.
    return TRUE;
}

// Sets the 'Play' state.
void set_play(Game* game)
{
    // TODO:
    // - Set the state field to PLAY.
    // - Set the label of the start button to "Pause".
    // - Disable the stop button.
    // - Set the on_move_disc() function to be called at regular intervals.
    game->state = PLAY;
    gtk_button_set_label(game->ui.start_button, "Pause");
    gtk_widget_set_sensitive(GTK_WIDGET(game->ui.stop_button), FALSE);
    if (game->disc.event == 0)
        game->disc.event = g_timeout_add(game->disc.period, on_move_disc, game);
}

// Sets the 'Pause' state.
void set_pause(Game* game)
{
    // TODO:
    // - Set the state field to PAUSE.
    // - Set the label of the start button to "Resume".
    // - Enable the stop button.
    // - Stop the on_move_disc() function.
    game->state = PAUSE;
    gtk_button_set_label(game->ui.start_button, "Resume");
    gtk_widget_set_sensitive(GTK_WIDGET(game->ui.stop_button), TRUE);
    if (game->disc.event != 0) {
        // Deactivates it.
        g_source_remove(game->disc.event);

        // Resets the event ID to zero.
        game->disc.event = 0;
    }
}

// Sets the 'Stop' state.
void set_stop(Game *game)
{
    // TODO:
    // - Set the state field to STOP.
    // - Set the label of the start button to "Start".
    // - Disable the stop button.
    game->state = STOP;
    gtk_button_set_label(game->ui.start_button, "Start");
    gtk_widget_set_sensitive(GTK_WIDGET(game->ui.stop_button), FALSE);
}

// Event handler for the "clicked" signal of the start button.
void on_start(GtkButton *button, gpointer user_data)
{
    Game* game = user_data;

    switch (game->state)
    {
        case STOP: set_play(game); break;
        case PLAY: set_pause(game); break;
        case PAUSE: set_play(game); break;
    };
}

// Event handler for the "clicked" signal of the stop button.
void on_stop(GtkButton *button, gpointer user_data)
{
    set_stop(user_data);
}

GdkPixbuf * gtk_image_new_from_sdl_surface (SDL_Surface *surface)
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


    return pixbuf;
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
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "org.pc2.ocr"));
    GtkImage* image = GTK_IMAGE(gtk_builder_get_object(builder, "img"));

    SDL_Surface *surf;
    surf = IMG_Load("../DataSample/original/image_03.jpeg");
    GdkPixbuf *pixbuf = gtk_image_new_from_sdl_surface(surf);
    gtk_image_set_from_pixbuf(image, pixbuf);
    g_object_unref (pixbuf);

    // Connects event handlers.
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show(GTK_WIDGET(window));
    // Runs the main loop.
    gtk_main();

    // Exits.
    return 0;
}
