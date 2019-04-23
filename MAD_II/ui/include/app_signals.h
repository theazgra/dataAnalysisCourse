#pragma once
#include <app_global.h>

// Helper functions
void attach_menu_item_activate(const NetworkWizardApplication &app, const gchar *uiElementId, void *callback);

void attach_signals(const NetworkWizardApplication &app);

// File submenu signals
void import_network_from_edge_pairs(GtkMenuItem *mi, gpointer data);
void import_network_from_vector_data(GtkMenuItem *mi, gpointer data);
void export_network(GtkMenuItem *mi, gpointer data);
void clean_network(GtkMenuItem *mi, gpointer data);
void exit_application(GtkMenuItem *mi, gpointer data);

// Generate
void generate_network(GtkMenuItem *mi, gpointer data);

// Network operations
void simulate_attack(GtkMenuItem *mi, gpointer data);
void simulate_failure(GtkMenuItem *mi, gpointer data);
void simulate_epidemic_SI(GtkMenuItem *mi, gpointer data);
void simulate_epidemic_SIS(GtkMenuItem *mi, gpointer data);
void simulate_epidemic_SIR(GtkMenuItem *mi, gpointer data);
void sampling_random_node(GtkMenuItem *mi, gpointer data);
void sampling_random_edge(GtkMenuItem *mi, gpointer data);
void k_core(GtkMenuItem *mi, gpointer data);
void kernighan_lin(GtkMenuItem *mi, gpointer data);
void hierarchical_clustering(GtkMenuItem *mi, gpointer data);