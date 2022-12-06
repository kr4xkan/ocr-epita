#include <gtk/gtk.h>

// Global variables to store the position of the widget
static double widget_x = 0;
static double widget_y = 0;

// Flag to indicate whether the widget is currently being dragged
static gboolean dragging = FALSE;

// Callback function for the "button-press-event" signal
static gboolean on_button_press(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  // Get the coordinates of the mouse pointer
  double x, y;
  gdk_event_get_coords(event, &x, &y);

  // Store the position of the widget
  widget_x = x;
  widget_y = y;

  // Set the dragging flag to TRUE
  dragging = TRUE;

  return TRUE;
}

// Callback function for the "button-release-event" signal
static gboolean on_button_release(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  // Set the dragging flag to FALSE
  dragging = FALSE;

  return TRUE;
}

// Callback function for the "motion-notify-event" signal
static gboolean on_motion_notify(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  // Check if the widget is currently being dragged
  if (dragging)
  {
    // Get the coordinates of the mouse pointer
    double x, y;
    gdk_event_get_coords(event, &x, &y);

    // Update the position of the widget
    widget_x += x - widget_x;
    widget_y += y - widget_y;
    gtk_layout_move(GTK_LAYOUT(data), widget, widget_x, widget_y);
  }

  return TRUE;
}




int main(int argc, char *argv[])
{
  GtkWidget *window;
  GtkWidget *layout;
  GtkWidget *widget;

  gtk_init(&argc, &argv);

  // Create a new window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  // Create a new layout container
  layout = gtk_layout_new(NULL, NULL);

  // Create a new widget
  widget = gtk_button_new_with_label("Drag Me!");

  // Enable mouse events for the widget
  gtk_widget_set_events(widget, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);

  // Connect the "button-press-event", "button-release-event", and "motion-notify-event" signals
  g_signal_connect(widget, "button-press-event", G_CALLBACK(on_button_press), NULL);
  g_signal_connect(widget, "button-release-event", G_CALLBACK(on_button_release), NULL);
  g_signal_connect(widget, "motion-notify-event", G_CALLBACK(on_motion_notify), layout);

  // Add the widget to the layout container
  gtk_layout_put(GTK_LAYOUT(layout), widget, widget_x, widget_y);

  // Add the layout container to the window
  gtk_container_add(GTK_CONTAINER(window), layout);

  // Show the window
  gtk_widget_show_all(window);

  // Connect the "destroy" signal to the gtk_main_quit() function
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // Enter the main loop
  gtk_main();

  return 0;
}

