#include <gtk/gtk.h>
#include <stdio.h>
//import_from_edge_pairs(GtkMenuItem *mi, gpointer data)
void import_from_edge_pairs(GtkWidget *widget, gpointer data)
{
    g_print("Import from edge pairs\n");
    printf("Import data from edge pairs called\n");
}

static void
print_hello(GtkWidget *widget, gpointer data)
{
    g_print("Hello World\n");
}

int main(int argc,
         char *argv[])
{
    GtkBuilder *builder;
    GObject *window;
    GObject *button;
    GError *error = NULL;

    gtk_init(&argc, &argv);

    /* Construct a GtkBuilder instance and load our UI description */
    builder = gtk_builder_new();
    if (gtk_builder_add_from_file(builder, "../mainWindow.ui", &error) == 0)
    {
        printf("ERROR\n");
        //

        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }
    else
    {
        printf("No Error\n");
    }

    // GObject *miImportFromEdgePairs = gtk_builder_get_object(builder, "miImportFromEdgePairs");
    // g_signal_connect(miImportFromEdgePairs, "activate", G_CALLBACK(import_from_edge_pairs), NULL);
    gtk_builder_connect_signals(builder, NULL);

    window = gtk_builder_get_object(builder, "windowMainWindow");
    gtk_widget_show(GTK_WIDGET(window));

    /*
    // Connect signal handlers to the constructed widgets.
    window = gtk_builder_get_object(builder, "window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    button = gtk_builder_get_object(builder, "button1");
    g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);

    button = gtk_builder_get_object(builder, "button2");
    g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);

    button = gtk_builder_get_object(builder, "quit");
    g_signal_connect(button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    */
    gtk_main();

    return 0;
}
