#include <app_signals.h>
#include <networkLib/io.h>

void attach_menu_item_activate(const NetworkWizardApplication *app, const gchar *uiElementId, void *callback)
{
    GObject *uiElement = app->get_object_by_id(uiElementId);
    g_signal_connect(uiElement, "activate", G_CALLBACK(callback), (void *)app);
}

gchar *open_file_dialog(const gchar *title, GtkWindow *parent)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;
    dialog = gtk_file_chooser_dialog_new(title, parent, action, ("_Cancel"), GTK_RESPONSE_CANCEL, ("_Open"), GTK_RESPONSE_ACCEPT, NULL);
    res = gtk_dialog_run(GTK_DIALOG(dialog));

    if (res == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        gchar *result = gtk_file_chooser_get_filename(chooser);
        gtk_widget_destroy(dialog);
        return result;
    }
    gtk_widget_destroy(dialog);
    return nullptr;
}

void attach_signals(const NetworkWizardApplication *app)
{
    // File menu
    attach_menu_item_activate(app, "miImportFromEdgePairs", (void *)import_network_from_edge_pairs);
    attach_menu_item_activate(app, "miImportVector", (void *)import_network_from_vector_data);
    attach_menu_item_activate(app, "miExport", (void *)export_network);
    attach_menu_item_activate(app, "miClear", (void *)clean_network);
    attach_menu_item_activate(app, "miClose", (void *)exit_application);

    // Generate menu
    attach_menu_item_activate(app, "miGenerate", (void *)generate_network);

    // Operations menu
    attach_menu_item_activate(app, "miSimAttack", (void *)simulate_attack);
    attach_menu_item_activate(app, "miSimFailure", (void *)simulate_failure);
    attach_menu_item_activate(app, "miSimEpidemicSI", (void *)simulate_epidemic_SI);
    attach_menu_item_activate(app, "miSimEpidemicSIS", (void *)simulate_epidemic_SIS);
    attach_menu_item_activate(app, "miSimEpidemicSIR", (void *)simulate_epidemic_SIR);
    attach_menu_item_activate(app, "miSamplingRandNode", (void *)sampling_random_node);
    attach_menu_item_activate(app, "miSamplingRandEdge", (void *)sampling_random_edge);
    attach_menu_item_activate(app, "miHierarchicalClustering", (void *)hierarchical_clustering);
    attach_menu_item_activate(app, "miKCore", (void *)k_core);
    attach_menu_item_activate(app, "miKernighanLin", (void *)kernighan_lin);
}

// File submenu signals
void import_network_from_edge_pairs(GtkMenuItem *mi, gpointer data)
{
    NetworkWizardApplication *app = (NetworkWizardApplication *)data;
    fprintf(stdout, "CALLED: import_network_from_edge_pairs\n");

    gchar *result = open_file_dialog("Import from edge pairs", app->get_main_window());
    if (result)
    {
        app->state.network = NetworkMatrix(result);
        app->recalculate_network_stats();
        fprintf(stdout, "Loaded network with %u vertices\n", app->state.network.vertex_count());
    }
}

void import_network_from_vector_data(GtkMenuItem *mi, gpointer data)
{
    NetworkWizardApplication *app = (NetworkWizardApplication *)data;
    fprintf(stdout, "CALLED: import_network_from_vector_data\n");

    // gchar *file;
    // if (open_file_dialog("Import from vector data", GTK_WINDOW(app->mainWindow), file))
    // {
    //     fprintf(stdout, "Chosen: %s\n", file);
    // }
}

void export_network(GtkMenuItem *mi, gpointer data)
{
    NetworkWizardApplication *app = (NetworkWizardApplication *)data;
    fprintf(stdout, "CALLED: export_network\n");
}

void clean_network(GtkMenuItem *mi, gpointer data)
{
    NetworkWizardApplication *app = (NetworkWizardApplication *)data;
    app->clear_network();
    fprintf(stdout, "CALLED: clean_network\n");
}

void exit_application(GtkMenuItem *mi, gpointer data)
{
    gtk_main_quit();
}

// Generate
void generate_network(GtkMenuItem *mi, gpointer data)
{
    fprintf(stdout, "CALLED: generate_network\n");
}

// Network operations
void simulate_attack(GtkMenuItem *mi, gpointer data)
{
    fprintf(stdout, "CALLED: simulate_attack\n");
}

void simulate_failure(GtkMenuItem *mi, gpointer data)
{
    fprintf(stdout, "CALLED: simulate_failure\n");
}

void simulate_epidemic_SI(GtkMenuItem *mi, gpointer data)
{
    fprintf(stdout, "CALLED: simulate_epidemic_SI\n");
}

void simulate_epidemic_SIS(GtkMenuItem *mi, gpointer data)
{
    fprintf(stdout, "CALLED: simulate_epidemic_SIS\n");
}

void simulate_epidemic_SIR(GtkMenuItem *mi, gpointer data)
{
    fprintf(stdout, "CALLED: simulate_epidemic_SIR\n");
}

void sampling_random_node(GtkMenuItem *mi, gpointer data)
{
    fprintf(stdout, "CALLED: sampling_random_node\n");
}

void sampling_random_edge(GtkMenuItem *mi, gpointer data)
{
    fprintf(stdout, "CALLED: sampling_random_edge\n");
}

void k_core(GtkMenuItem *mi, gpointer data)
{
    fprintf(stdout, "CALLED: k_core\n");
}

void kernighan_lin(GtkMenuItem *mi, gpointer data)
{
    fprintf(stdout, "CALLED: kernighan_lin\n");
}

void hierarchical_clustering(GtkMenuItem *mi, gpointer data)
{
    fprintf(stdout, "CALLED: hierarchical_clustering\n");
}