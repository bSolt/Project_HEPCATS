#include <gtk/gtk.h>
#include <vte/vte.h>

int 
main (int argc, char *argv[])
{
//Define all GUI object pointers
GtkBuilder *builder;
GObject *window;
//terminals
GObject *term_prompt_cmd, *term_gnd_msg, *term_cmd_counts, *term_health_telem, *term_sci_telem;
GObject *term_preset_cmd;
GObject *txt_box_msg;

//RadioButtons
GObject *rad_btn_master;
GObject *rad_btn_monitor;

//Buttons
GObject *btn_send_cmd; 

//Send init and get the builder
gtk_init(&argc, &argv);
builder=gtk_builder_new();
gtk_builder_add_from_file (builder, "GroundControlInterface.glade", NULL);

//Get all objects from the builder
window=gtk_builder_get_object (builder, "window");
g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);



gtk_main();

return 0;

}

