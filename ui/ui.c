#include <gtk/gtk.h>
#include "../cutter/crop-manager.h"
#include "../cutter/cutter.h"





// Event handler for the "clicked" signal of the start button.
void on_run(GtkButton *button, gpointer user_data)
{

}

// Event handler for the "clicked" signal of the stop button.
void on_confirm(GtkButton *button, gpointer user_data)
{

}

void on_move(GtkRadioButton *button, GtkLayout *layout)
{
    gtk_layout_move(layout, GTK_WIDGET(button), 10, 10);
    printf("aaaaaaaaaaaaaaaaaaaaaa\n");

}

void on_stop(GtkRadioButton *button, gpointer user_data)
{
    printf("bbbbbbbbbb\n");

}




gboolean on_configure(GtkWidget *widget, GdkEvent *event, gpointer user_data){


    return FALSE;
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
    if (gtk_builder_add_from_file(builder, "ui.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    // Gets the widgets.
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "test"));
    GtkLayout* layout = GTK_LAYOUT(gtk_builder_get_object(builder, "layout"));
    GtkButton* run_button = GTK_BUTTON(gtk_builder_get_object(builder, "run_button"));
    GtkButton* confirm_button = GTK_BUTTON(gtk_builder_get_object(builder, "confirm_button"));

    GtkRadioButton* top_left = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "top_left"));

    gtk_layout_put(GTK_LAYOUT(layout), GTK_WIDGET(top_left), 10, 10);
    gtk_layout_set_size(GTK_LAYOUT(layout), 10, 10);

    // Connects event handlers.
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(run_button, "clicked", G_CALLBACK(on_run), NULL);
    g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_confirm), NULL);
    g_signal_connect(top_left, "pressed", G_CALLBACK(on_move), &layout);
    g_signal_connect(top_left, "released", G_CALLBACK(on_stop), &layout);


    // Runs the main loop.
    gtk_main();

    // Exits.
    return 0;
}
