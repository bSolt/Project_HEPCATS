#include <gtk/gtk.h>
#include <stdlib.h>


static void 
print_success (GtkWidget *widget, 
	gpointer data)
{
	g_print ("Comms test was a success\n");
}

static void
spawnSerial (GtkWidget *widget,
	gpointer data)
{
GError *error = NULL;
int exit_status=0;
char *output=NULL;
char *arg[] = {"serial_comm_test.sh",NULL};
	g_spawn_sync(NULL, arg, NULL, 0,
 NULL, NULL, &output, NULL, &exit_status, &error);
}
int
main (int argc,
	char *argv[])
{
	GtkBuilder *builder;
	GObject *window;
	GObject *button;
	GError *error = NULL;

	gtk_init (&argc, &argv);

	builder=gtk_builder_new ();
	if (gtk_builder_add_from_file (builder, "SerialGUItest.glade", &error) == 0)
	{
	g_printerr ("Error loading the GUI: %s\n", error->message);
	g_clear_error (&error);
	return 1;
	}

	window=gtk_builder_get_object (builder, "MainWindow");
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	
	button=gtk_builder_get_object (builder, "testButton");
	g_signal_connect (button, "clicked", G_CALLBACK (spawnSerial), NULL);

	gtk_main ();
	
	return 0;

}
