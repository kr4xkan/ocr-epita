#include "ui.h"

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

    float nW;
    float nH;
    if (preserve_ratio == 1) {
        nW = 580;
        nH = surface->h * nW / surface->w;
    } else if (preserve_ratio == 2) {
        float ws = 330;
        float hs = 260;
        float rs = ws/hs;
        float ri = surface->w/surface->h;
        if (rs > ri) {
            nW = surface->w * (hs/surface->h);
            nH = hs;
        } else {
            nW = ws;
            nH = surface->h * (ws/surface->w);
        }
    } else {
        float ws = 580;
        float hs = 400;
        float rs = ws/hs;
        float ri = surface->w/surface->h;
        if (rs > ri) {
            nW = surface->w * (hs/surface->h);
            nH = hs;
        } else {
            nW = ws;
            nH = surface->h * (ws/surface->w);
        }
    }

    GdkPixbuf* pxbscaled = gdk_pixbuf_scale_simple(pixbuf, nW, nH, GDK_INTERP_BILINEAR);
    
    gtk_image_set_from_pixbuf(img_container, pxbscaled);

    g_object_unref (pixbuf);
    g_object_unref (pxbscaled);
    return (double)nW/(double)surface->w;
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
    set_gtk_image_from_surface(app_state->img_preprocessing, surf, 0);
    app_state->current_surf = surf;
    app_state->rotate_surf = RotateSurface(app_state->current_surf, 0);

    gtk_widget_show(GTK_WIDGET(app_state->w_preprocessing));
    gtk_widget_show(GTK_WIDGET(app_state->next_preprocessing));
    gtk_widget_hide(GTK_WIDGET(app_state->w_home));
}

void on_next_preprocessing(GtkButton* self, gpointer user_data) {
    AppState* app_state = user_data;
    gtk_widget_show(GTK_WIDGET(app_state->w_lines));
    gtk_widget_show(GTK_WIDGET(app_state->next_lines));
    gtk_widget_hide(GTK_WIDGET(app_state->w_preprocessing));
    app_state->current_surf = app_state->rotate_surf;

    app_state->draw.ratio = set_gtk_image_from_surface(app_state->img_lines, app_state->current_surf, 1);

    on_run(app_state);
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

    Intersection* corners = calloc(4, sizeof(Intersection));
    corners[0].x = app_state->draw.p1.x/app_state->draw.ratio;
    corners[0].y = app_state->draw.p1.y/app_state->draw.ratio;
    corners[1].x = app_state->draw.p2.x/app_state->draw.ratio;
    corners[1].y = app_state->draw.p2.y/app_state->draw.ratio;
    corners[2].x = app_state->draw.p3.x/app_state->draw.ratio;
    corners[2].y = app_state->draw.p3.y/app_state->draw.ratio;
    corners[3].x = app_state->draw.p4.x/app_state->draw.ratio;
    corners[3].y = app_state->draw.p4.y/app_state->draw.ratio;

    app_state->cells = ManualCutter(app_state->current_surf, corners);

    char grid[9][9] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
    };
    for (size_t i = 0; i < 81; i++) {
        size_t x = i / 9;
        size_t y = i % 9;
        grid[x][y] = recognize_digit(app_state->cells[i]);
        free(app_state->cells[i]);
    }
    free(app_state->cells);

    char changed[9][9] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
    };
    SDL_Surface* grid_surface = make_sudoku_grid(grid, changed);
    set_gtk_image_from_surface(app_state->img_neural_res, grid_surface, 2);
    SDL_FreeSurface(grid_surface);
    set_gtk_image_from_surface(app_state->img_neural, app_state->current_surf, 2);
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

// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
// DO SOLVER + SHOW RESOLVED GRID
    char grid[9][9] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
        {0, 1, 2, 3, 4, 5, 6, 7, 8},
    };
    char changed[9][9] = {
        {0, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 0, 0, 0, 1, 1, 1},
        {0, 0, 1, 0, 1, 1, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0},
        {1, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
    };

    SDL_Surface* grid_surface = make_sudoku_grid(grid, changed);
    set_gtk_image_from_surface(app_state->img_solver, grid_surface, 0);
    SDL_FreeSurface(grid_surface);
}

void on_next_solver(GtkButton* self, gpointer user_data) {
    AppState* app_state = user_data;
    gtk_widget_show(GTK_WIDGET(app_state->w_home));
    gtk_widget_hide(GTK_WIDGET(app_state->w_solver));
}

void on_rotate_image(GtkRange* slider, gpointer user_data) {
    AppState* app_state = user_data;

    double angle = gtk_range_get_value(slider);
    if (app_state->rotate_surf)
        SDL_FreeSurface(app_state->rotate_surf);
    app_state->rotate_surf = RotateSurface(app_state->current_surf, angle);
    set_gtk_image_from_surface(app_state->img_preprocessing, app_state->rotate_surf, 0);
}

SDL_Surface* make_sudoku_grid(char grid[9][9], char changed[9][9]) {
    TTF_Font* Sans = TTF_OpenFont("./Sans.ttf", 24);
    SDL_Color Black = {0, 0, 0};
    SDL_Color Red = {66, 135, 245};
    SDL_Surface *surf = IMG_Load("./blank.png");

    // Write TEXT
    for (size_t i = 0; i < 9; i++) {
        for (size_t j = 0; j < 9; j++) {
            if (grid[i][j] != 0) {
                char text[2] = { 0, 0 };
                text[0] = grid[i][j] + 48;
                SDL_Surface* surfaceMessage;
                if (changed[i][j])
                    surfaceMessage = TTF_RenderText_Solid(Sans, text, Red);
                else
                    surfaceMessage = TTF_RenderText_Solid(Sans, text, Black);
                SDL_Rect Message_rect;
                Message_rect.x = 24 + (i * 48) + (i > 2) * 6;
                Message_rect.y = 14 + (j * 48) - (j > 2) * 6;
                Message_rect.w = surfaceMessage->w;
                Message_rect.h = surfaceMessage->h;
                SDL_BlitSurface(surfaceMessage, NULL, surf, &Message_rect);
                SDL_FreeSurface(surfaceMessage);
            }
        }
    }

    TTF_CloseFont(Sans);
    return surf;
}

int main (int argc, char *argv[])
{
    TTF_Init();
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
    GtkImage* img_neural_res = GTK_IMAGE(
            gtk_builder_get_object(builder, "img_neural_res"));
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
    GtkScale* slider_rotate = GTK_SCALE(
            gtk_builder_get_object(builder, "slider_rotate"));
    gtk_range_set_range(GTK_RANGE(slider_rotate), 0, 180);

    GtkDrawingArea* area = GTK_DRAWING_AREA(
            gtk_builder_get_object(builder, "area"));

    AppState app_state = {
        .w_home = w_home,
        .w_preprocessing = w_preprocessing,
        .w_lines = w_lines,
        .w_neural = w_neural,
        .w_solver = w_solver,
        .img_preprocessing = img_preprocessing,
        .img_lines = img_lines,
        .img_neural = img_neural,
        .img_neural_res = img_neural_res,
        .img_solver = img_solver,
        .next_preprocessing = next_preprocessing,
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

    // Connects event handlers.
    gtk_widget_add_events (GTK_WIDGET(area), GDK_BUTTON_PRESS_MASK);
    gtk_widget_add_events (GTK_WIDGET(area), GDK_BUTTON_RELEASE_MASK);
    gtk_widget_add_events (GTK_WIDGET(area), GDK_POINTER_MOTION_MASK);
    g_signal_connect(area, "draw", G_CALLBACK(on_draw), &app_state);
    g_signal_connect(area, "button-press-event", G_CALLBACK(on_press), &app_state);
    g_signal_connect(area, "button-release-event", G_CALLBACK(on_release), &app_state);
    g_signal_connect(area, "motion-notify-event", G_CALLBACK(on_cursor_motion), &app_state);

    g_signal_connect(file_chooser, "file-set", G_CALLBACK(on_select_file), &app_state);
    g_signal_connect(next_preprocessing, "clicked", G_CALLBACK(on_next_preprocessing), &app_state);
    g_signal_connect(next_lines, "clicked", G_CALLBACK(on_next_lines), &app_state);
    g_signal_connect(next_neural, "clicked", G_CALLBACK(on_next_neural), &app_state);
    g_signal_connect(next_solver, "clicked", G_CALLBACK(on_next_solver), &app_state);
    g_signal_connect(GTK_RANGE(slider_rotate), "value-changed", G_CALLBACK(on_rotate_image), &app_state);
    g_signal_connect(w_home, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(w_preprocessing, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(w_lines, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(w_neural, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(w_solver, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show(GTK_WIDGET(w_home));
    // Runs the main loop.
    gtk_main();
    TTF_Quit();

    // Exits.
    return 0;
}
