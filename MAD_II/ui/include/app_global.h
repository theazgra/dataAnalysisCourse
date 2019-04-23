#pragma once
#include <stdio.h>
#include <string>
#include <gtk/gtk.h>

struct NetworkInformations
{
    uint vertexCount;
    uint edgeCount;
    float averageDegree;
    float averageClusteringCoefficient;
    float averageDistance;
    uint diameter;
    uint componentCount;
    uint maximumComponentSize;
};

struct ApplicationState
{
    uint age = 5;
    NetworkInformations network;
};

// Convert to class.
struct NetworkWizardApplication
{
    GtkBuilder *uiBuilder;
    GObject *mainWindow;
    ApplicationState state;

    void show_app_window(const char *mainWindowId)
    {
        mainWindow = gtk_builder_get_object(uiBuilder, mainWindowId);
        gtk_widget_show(GTK_WIDGET(mainWindow));

        state.network.vertexCount = 1000;
        state.network.edgeCount = 1500;
        state.network.averageDegree = 15.23;
        state.network.averageClusteringCoefficient = 0.484;
        state.network.averageDistance = 3.25;
        state.network.diameter = 7;
        state.network.componentCount = 4;
        state.network.maximumComponentSize = 265;

        update_network_informations();
    }

    void clear_network()
    {
        state.network.vertexCount = 0;
        state.network.edgeCount = 0;
        state.network.averageDegree = 0;
        state.network.averageClusteringCoefficient = 0;
        state.network.averageDistance = 0;
        state.network.diameter = 0;
        state.network.componentCount = 0;
        state.network.maximumComponentSize = 0;

        update_network_informations();
    }

    void update_network_informations()
    {
        GObject *tbVertexCount = gtk_builder_get_object(uiBuilder, "tbVertexCount");
        GObject *tbEdgeCount = gtk_builder_get_object(uiBuilder, "tbEdgeCount");
        GObject *tbAvgDeg = gtk_builder_get_object(uiBuilder, "tbAvgDegree");
        GObject *tbAvgClustCoef = gtk_builder_get_object(uiBuilder, "tbAvgClusturingCoeff");
        GObject *tbAvgDist = gtk_builder_get_object(uiBuilder, "tbAvgDistance");
        GObject *tbDiam = gtk_builder_get_object(uiBuilder, "tbDiameter");
        GObject *tbCompCount = gtk_builder_get_object(uiBuilder, "tbComponentCount");
        GObject *tbMaxComp = gtk_builder_get_object(uiBuilder, "tbMaxComponentSize");

        gtk_entry_set_text(GTK_ENTRY(tbVertexCount), std::to_string(state.network.vertexCount).c_str());
        gtk_entry_set_text(GTK_ENTRY(tbEdgeCount), std::to_string(state.network.edgeCount).c_str());
        gtk_entry_set_text(GTK_ENTRY(tbAvgDeg), std::to_string(state.network.averageDegree).c_str());
        gtk_entry_set_text(GTK_ENTRY(tbAvgClustCoef), std::to_string(state.network.averageClusteringCoefficient).c_str());
        gtk_entry_set_text(GTK_ENTRY(tbAvgDist), std::to_string(state.network.averageDistance).c_str());
        gtk_entry_set_text(GTK_ENTRY(tbDiam), std::to_string(state.network.diameter).c_str());
        gtk_entry_set_text(GTK_ENTRY(tbCompCount), std::to_string(state.network.componentCount).c_str());
        gtk_entry_set_text(GTK_ENTRY(tbMaxComp), std::to_string(state.network.maximumComponentSize).c_str());
    }
};