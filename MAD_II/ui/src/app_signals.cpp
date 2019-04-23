#include <app_signals.h>

void attach_menu_item_activate(const NetworkWizardApplication &app, const gchar *uiElementId, void *callback)
{
    GObject *uiElement = gtk_builder_get_object(app.uiBuilder, uiElementId);
    g_signal_connect(uiElement, "activate", G_CALLBACK(callback), (void *)&app);
}

void attach_signals(const NetworkWizardApplication &app)
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
}

void import_network_from_vector_data(GtkMenuItem *mi, gpointer data)
{
    NetworkWizardApplication *app = (NetworkWizardApplication *)data;
    fprintf(stdout, "CALLED: import_network_from_vector_data\n");
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