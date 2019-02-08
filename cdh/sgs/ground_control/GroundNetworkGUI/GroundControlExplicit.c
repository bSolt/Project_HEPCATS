#include <gtk/gtk.h>
//#include <vte/vte.h>

int main(int argc, char *argv[])
{
GtkWidget* window;
GtkWidget* button;
GtkWidget* label;

gtk_init(&argc, &argv);

window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
button=gtk_button_new();
label=gtk_label_new("Fuck me");

gtk_container_add(GTK_CONTAINER(button),label);
gtk_container_add(GTK_CONTAINER(window),label);
g_signal_connect(GTK_OBJECT(window),"delete_event", GTK_SIGNAL_FUNC(delete_event_cb),NULL);

gtk_widget_show_all(window);

gtk_main();
return 0;

}

static gint
delete_event_cb(GtkWindow* window, GdkEventAny* e, gpointer data)
{
	gtk_main_quit();
	return FALSE;
}

