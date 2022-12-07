#include <gtk/gtk.h>

//global variables
GtkWindow *opening_window;
GtkWindow *window2;

GtkWidget *fixed_container_1;
GtkPaned *paned;

GtkButton *open_button;

GtkLabel *label1;
GtkBuilder *builder;
GtkFileChooserButton *file_chooser;



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

gchar* select_file(GtkWindow *window, GdkEvent *event, gpointer file_chooser){
	return gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(file_chooser));
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
	label1 = GTK_LABEL(gtk_builder_get_object(builder, "label1"));
	file_chooser = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "file_chooser"));
	open_button =  GTK_BUTTON(gtk_builder_get_object(builder, "open_button"));
	paned = GTK_PANED(gtk_builder_get_object(builder, "paned"));
	area = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "area"));


	//signals
	g_signal_connect(opening_window, "destroy", G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(window2, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(opening_window, "configure-event", G_CALLBACK(on_configure_opening), &fixed_container_1);
	//g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_button), &);
	g_signal_connect(file_chooser, "file-set", G_CALLBACK(select_file), &file_chooser);

	//show windows
	gtk_widget_hide(GTK_WIDGET(window2));
	gtk_widget_show(GTK_WIDGET(opening_window));
	gtk_main();
	
	return 0;
}
