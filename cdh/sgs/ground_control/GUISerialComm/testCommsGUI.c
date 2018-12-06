//////////////////////////////////////////////////////////
//
// Simple GUI test
//
//-------------------------------------------------------
//
//Christopher Peercy
//ASEN 4018
//Project HEPCATS
//Subsystem: CDH
//Created: Nov 10 2018
// 
//Creates a simple GUI with a single button and demonstrates calling a .sh
//script with GTK

#include <gtk/gtk.h>

//Create a handler for the test button
static void
spawnSerial (GtkWidget *widget,
	gpointer data)
{
//Get pointers to handle error and output from the script
	GError *error = NULL;
	int exit_status=0;
	char *output=NULL;
	char *arg[] = {"serial_comm_test.sh",NULL}; //this is the script we want to call as well
// as it's argument vector
//Call a syncronous function handler to launch the script
	g_spawn_sync(NULL, arg, NULL, 0,
 		NULL, NULL, &output, NULL, &exit_status, &error);
}

int
main (int argc,
	char *argv[])
{
//Create pointers for the GTK objects we'll be calling
	GtkBuilder *builder;
	GObject *window;
	GObject *button;
	GError *error = NULL;

	gtk_init (&argc, &argv);
//Make a new GTK builder object from the glade file we have
	builder=gtk_builder_new ();
	if (gtk_builder_add_from_file (builder, "SerialGUItest.glade", &error) == 0)
	{
	g_printerr ("Error loading the GUI: %s\n", error->message);
	g_clear_error (&error);
	return 1;
	}
//Define the window destroy signal
	window=gtk_builder_get_object (builder, "MainWindow");
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
//Define what action should be performed when button is clicked (callback to spawnSerial)
	button=gtk_builder_get_object (builder, "testButton");
	g_signal_connect (button, "clicked", G_CALLBACK (spawnSerial), NULL);
//Execute gtk_main
	gtk_main ();

	return 0;

}
