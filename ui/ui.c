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
#include "drawarea.h"
#include "types.h"
#include "ui.h"



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
SDL_Rect rect;
SDL_Surface *void_grid;

SDL_Surface *numb_solver;



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


gchar* select_file(GtkWindow *window){
    surf_rot = loadImage(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser)));
    tmp = surf_rot;
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    return filename;
}



void load_number(int number_grid){
    if (numb_solver)
        SDL_FreeSurface(numb_solver);
   switch (number_grid)
   {
	case 1:
		numb_solver = loadImage("../solver/chiffres/1.jpg"); 
   		break;
	case 2:
		numb_solver = loadImage("../solver/chiffres/2.jpg");
   		break;
   	case 3:
		numb_solver = loadImage("../solver/chiffres/3.jpg"); 
   		break;
   	case 4:
		numb_solver = loadImage("../solver/chiffres/4.jpg"); 
   		break;
   	case 5:
		numb_solver = loadImage("../solver/chiffres/5.jpg");
		break;
	case 6:
		numb_solver = loadImage("../solver/chiffres/6.jpg"); 
   		break;
	case 7:
		numb_solver = loadImage("../solver/chiffres/7.jpg"); 
   		break;
	case 8:
		numb_solver = loadImage("../solver/chiffres/8.jpg"); 
   		break;
   	case 9:
		numb_solver = loadImage("../solver/chiffres/9.jpg"); 
   		break;

   }
}

//recupere l'array de 10 10 de la fonction solver
void grid_to_image(int grid[9][9]){

	void_grid = LoadImage("../void_grid.jpg");

	for (int i = 0; i<9 ; i++){
		for (int j = 0; j<9 ; j++) {
            if (grid[i][j]) {
                load_number(grid[i][j]);
                SDL_BlitScaled(numb_solver, NULL, void_grid, &rect);
            }

            if(rect.y + 67 >= 600){
                rect.y = 15;
            }
            else{
                rect.y = rect.y + 67;
            }
		}
	    rect.x = i * 62 + 83 + (i>1) * 10 + (i>3) * 10;
	}				
}

//array pour tester la fonction grid_to_image
int grid_ex[9][9] = {
        {1,2,3,4,5,6,7,8,9},
		{1,2,3,4,5,6,7,8,9},
		{1,2,3,4,5,6,7,8,9},

		{1,2,3,4,5,6,7,8,9},
		{1,2,3,4,5,6,7,8,9},
		{1,2,3,4,5,6,7,8,9},
		
		{1,2,3,4,5,6,7,8,9},
		{1,2,3,4,5,6,7,8,9},
		{1,2,3,4,5,6,7,8,9}};

void on_open_button(GtkWidget *widget){
    set_gtk_image_from_surface(image, surf_rot, 0);

    gtk_widget_hide(GTK_WIDGET(opening_window));
    gtk_widget_show(GTK_WIDGET(manual_rotation));
}

void on_rotate(GtkRange *widget){
    double angle = gtk_range_get_value(widget);
    tmp = RotateSurface(surf_rot, angle);
    set_gtk_image_from_surface(image, tmp, 0);
}

void on_next_button_rot(GtkWidget *widget){
    //binarisation
    surf_rot = tmp;
    IMG_SavePNG(surf_rot, "rotated.png");
    binarization(surf_rot, 10);
    SDL_FreeSurface(surf_rot);
    surf_rot = loadImage("rotated.png");
    surf_bin = loadImage("binary.png");
    set_gtk_image_from_surface(bin_image, surf_bin, 0);

    gtk_widget_hide(GTK_WIDGET(manual_rotation));
    gtk_toggle_button_set_active(binarize_check, TRUE);
    gtk_widget_show(GTK_WIDGET(binarisation));
}

void on_binarize_check(GtkWidget *widget){
    gboolean checked = gtk_toggle_button_get_active(binarize_check);
    if(checked){
        set_gtk_image_from_surface(bin_image, surf_bin, 0);
    }
    else{
        set_gtk_image_from_surface(bin_image, surf_rot, 0);
    }
}


void on_next_button_bin(GtkWidget *widget, gpointer user_data){
    // partie de romain
    AppState *app_state = user_data;

    gtk_widget_show(GTK_WIDGET(app_state->line_check_window));
    gtk_widget_show(GTK_WIDGET(app_state->next_lines));
    gtk_widget_hide(GTK_WIDGET(binarisation));

    app_state->current_surface = surf_bin;
    app_state->draw.ratio = set_gtk_image_from_surface(app_state->img_lines, app_state->current_surface, 1);

    Run(app_state);
}

void on_next_button_cutter(GtkWidget *widget, gpointer user_data){
	AppState *app_state = user_data;
//neural-network 
//neural-network 
//neural-network 
//neural-network 
    Intersection* corners = calloc(4, sizeof(Intersection));
    corners[0].x = app_state->draw.p1.x/app_state->draw.ratio;
    corners[0].y = app_state->draw.p1.y/app_state->draw.ratio;
    corners[1].x = app_state->draw.p2.x/app_state->draw.ratio;
    corners[1].y = app_state->draw.p2.y/app_state->draw.ratio;
    corners[2].x = app_state->draw.p3.x/app_state->draw.ratio;
    corners[2].y = app_state->draw.p3.y/app_state->draw.ratio;
    corners[3].x = app_state->draw.p4.x/app_state->draw.ratio;
    corners[3].y = app_state->draw.p4.y/app_state->draw.ratio;
   
    app_state->cells = ManualCutter(app_state->current_surface, corners);
   
    for (size_t i = 0; i < 81; i++) {
        size_t x = i / 9;
        size_t y = i % 9;
        app_state->sudoku[x][y] = recognize_digit(app_state->cells[i]);
        free(app_state->cells[i]);
    }
    free(app_state->cells); 

    for (size_t y = 0; y < 9; y++) {
        for (size_t x = 0; x < 9; x++) {
            printf("%d ", app_state->sudoku[x][y]);
        }
        printf("\n");
    }
  
    //int res = solver(app_state->sudoku, 0, 0);

//add the solver function to the grid from the nn
//
//not tried cause errors in cutter part
    grid_to_image(app_state->sudoku);
    set_gtk_image_from_surface(image3, void_grid, 0);

	gtk_widget_hide(GTK_WIDGET(app_state->line_check_window));
	gtk_widget_show(GTK_WIDGET(solved));
}

void on_save_button(GtkWidget *widget){
	IMG_SavePNG(void_grid, "solved_sudoku.png");
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget* dialog = gtk_message_dialog_new (solved,
                                     flags,
                                     GTK_MESSAGE_INFO,
                                     GTK_BUTTONS_CLOSE,
                                     "Solved Sudoku saved at:\n./solved_sudoku.png");
    g_signal_connect_swapped (dialog, "response",
                              G_CALLBACK (gtk_widget_destroy),
                              dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
}

void on_return_home(GtkWidget *widget) {
    SDL_FreeSurface(void_grid);
	gtk_widget_hide(GTK_WIDGET(solved));
	gtk_widget_show(GTK_WIDGET(opening_window));
}

int main(){

    gtk_init(NULL, NULL);

    builder = gtk_builder_new_from_file("window_ui.glade");

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
    
    GtkScale* angle_slider = GTK_SCALE(
        gtk_builder_get_object(builder, "angle_entry"));
    gtk_range_set_range(GTK_RANGE(angle_slider), 0, 180);

    //binarisation components
    binarisation = GTK_WINDOW(gtk_builder_get_object(builder, "binarisation"));
    fixed_container_bin = GTK_FIXED(gtk_builder_get_object(builder, "fixed_container_bin"));
    bin_image = GTK_IMAGE(gtk_builder_get_object(builder, "bin_image"));
    binarize_check = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "binarize_check"));
    next_button_bin = GTK_BUTTON(gtk_builder_get_object(builder,"next_button_bin"));

    next_button_cutter = GTK_BUTTON(gtk_builder_get_object(builder,"next_button_cutter"));



    //cutter components
    GtkWindow *line_check_window = GTK_WINDOW(gtk_builder_get_object(builder, "line_check_window"));
    GtkDrawingArea *area = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "area"));
    GtkImage *img_lines = GTK_IMAGE(gtk_builder_get_object(builder, "img_lines"));
    GtkButton *next_lines =  GTK_BUTTON(gtk_builder_get_object(builder, "next_lines"));

    AppState app_state = {
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



    //solved components
    solved = GTK_WINDOW(gtk_builder_get_object(builder,"solved"));
    fixed_container3 = GTK_FIXED(gtk_builder_get_object(builder, "fixed_container3"));
    image3 = GTK_IMAGE(gtk_builder_get_object(builder, "image3"));
    save_button = GTK_BUTTON(gtk_builder_get_object(builder, "save_button"));
    GtkButton *return_button =  GTK_BUTTON(gtk_builder_get_object(builder, "return_button"));
    rect.x = 15;
    rect.y = 15;
    rect.h = 40;
    rect.w = 40;

    //signals
    //WINDOW
    g_signal_connect(opening_window, "destroy", G_CALLBACK(gtk_main_quit),NULL);
    g_signal_connect(manual_rotation, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(binarisation, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(solved, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(line_check_window, "destroy",G_CALLBACK(gtk_main_quit),NULL);
    //
    //OPEN
    //
    g_signal_connect(file_chooser, "file-set", G_CALLBACK(select_file),NULL);
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_button),NULL);
    g_signal_connect(next_button_rot, "clicked", G_CALLBACK(on_next_button_rot), NULL);

    //
    //ROT
    //
    //g_signal_connect(angle_entry, "changed", G_CALLBACK(on_rotate), NULL);
    g_signal_connect(GTK_RANGE(angle_slider), "value-changed", G_CALLBACK(on_rotate), &app_state);
    //
    //BIN
    //
    g_signal_connect(binarize_check, "toggled", G_CALLBACK(on_binarize_check),NULL);
    g_signal_connect(next_button_bin, "clicked", G_CALLBACK(on_next_button_bin),&app_state);

    //
    //ROMAIN
    //
    gtk_widget_add_events (GTK_WIDGET(area), GDK_BUTTON_PRESS_MASK);
    gtk_widget_add_events (GTK_WIDGET(area), GDK_BUTTON_RELEASE_MASK);
    gtk_widget_add_events (GTK_WIDGET(area), GDK_POINTER_MOTION_MASK);
    g_signal_connect(area, "draw", G_CALLBACK(on_draw), &app_state);
    g_signal_connect(area, "button-press-event", G_CALLBACK(on_press), &app_state);
    g_signal_connect(area, "button-release-event", G_CALLBACK(on_release), &app_state);
    g_signal_connect(area, "motion-notify-event", G_CALLBACK(on_cursor_motion), &app_state);

    g_signal_connect(next_lines, "clicked", G_CALLBACK(on_next_button_cutter), &app_state);

    //
    //
    //

    g_signal_connect(return_button, "clicked", G_CALLBACK(on_return_home), NULL);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button), NULL);

    //show windows
    gtk_widget_hide(GTK_WIDGET(binarisation));
    gtk_widget_hide(GTK_WIDGET(solved));
    gtk_widget_hide(GTK_WIDGET(manual_rotation));
    gtk_widget_show(GTK_WIDGET(opening_window));
    gtk_main();

    return 0;
}
