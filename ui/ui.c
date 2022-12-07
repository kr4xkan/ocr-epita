#include <gtk/gtk.h>

//global variables
GtkWindow *opening_window;
GtkWindow *window2;

GtkWidget *fixed_container_1;
GtkPaned *paned;

GtkButton *button1;
GtkButton *open_button;

GtkLabel *label1;
GtkBuilder *builder;
GtkEntry *text_entry;

GtkDrawingArea *area;


//configure-event
gboolean on_configure_opening(GtkWidget *widget, GdkEvent *event, gpointer user_data){
	
	GtkFixed *fixed = user_data;

	gint width = gtk_widget_get_allocated_width(GTK_WIDGET(opening_window));
	gint height = gtk_widget_get_allocated_height(GTK_WIDGET(opening_window));

	//gtk_fixed_move(fixed, fixed->button1, width/5, height/2);
	//gtk_fixed_move(fixed_container, GTK_WIDGET(label1), width/2, 4*height/5);
	//gtk_fixed_move(fixed_container, GTK_WIDGET(text_entry), width/2, height/2);

	return FALSE;
}

//draws the image
gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data){
	cairo_set_source_rgb(cr,1,1,1);
	cairo_paint(cr);

}


//gets the filepath and hide open window
const gchar* get_entry(){
	return gtk_entry_get_text(text_entry);
}
	


gboolean on_open_button(GtkWidget *widget, cairo_t *cr, gpointer user_data){
	gchar *path = malloc(128 * sizeof(char));
	strcpy(path, get_entry());
	FILE *im = fopen(path, "r");
	if (!im){
		gtk_label_set_text(GTK_LABEL(label1),(const gchar*) "Could Not Load Image");
		return FALSE;
	}
	gtk_widget_hide(GTK_WIDGET(opening_window));
	gtk_widget_show(GTK_WIDGET(window2));
	user_data = im;
	return TRUE;
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
	opening_window = GTK_WINDOW(gtk_builder_get_object(builder, "opening_window"));
	window2 = GTK_WINDOW(gtk_builder_get_object(builder, "window2"));
	fixed_container_1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed_container_1"));
	button1 = GTK_BUTTON(gtk_builder_get_object(builder, "button1"));
	label1 = GTK_LABEL(gtk_builder_get_object(builder, "label1"));
	text_entry = GTK_ENTRY(gtk_builder_get_object(builder, "text_entry"));
	open_button =  GTK_BUTTON(gtk_builder_get_object(builder, "open_button"));
	paned = GTK_PANED(gtk_builder_get_object(builder, "paned"));
	area = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "area"));
	FILE* im;
	//signals
	g_signal_connect(opening_window, "destroy", G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(window2, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(opening_window, "configure-event", G_CALLBACK(on_configure_opening), &fixed_container_1);
	g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_button), &im);
	g_signal_connect(area, "draw", G_CALLBACK(on_draw), &area);

	//show windows
	gtk_widget_hide(GTK_WIDGET(window2));
	gtk_widget_show(GTK_WIDGET(opening_window));
	gtk_main();
	
	return 0;
}
