#include <network_wizard_app.h>

NetworkWizardApplication::NetworkWizardApplication(GtkBuilder *builder, const char *mainWindowId)
{
    this->uiBuilder = builder;
    this->mainWindow = gtk_builder_get_object(uiBuilder, mainWindowId);

    attach_signals(this);

    gtk_widget_show(GTK_WIDGET(mainWindow));

    this->state.networkInfoToShow.vertexCount = 1000;
    this->state.networkInfoToShow.edgeCount = 1500;
    this->state.networkInfoToShow.averageDegree = 15.23;
    this->state.networkInfoToShow.averageClusteringCoefficient = 0.484;
    this->state.networkInfoToShow.averageDistance = 3.25;
    this->state.networkInfoToShow.diameter = 7;
    this->state.networkInfoToShow.componentCount = 4;
    this->state.networkInfoToShow.maximumComponentSize = 265;

    update_network_informations();
}

void NetworkWizardApplication::clear_network()
{
    this->state.networkInfoToShow.vertexCount = 0;
    this->state.networkInfoToShow.edgeCount = 0;
    this->state.networkInfoToShow.averageDegree = 0;
    this->state.networkInfoToShow.averageClusteringCoefficient = 0;
    this->state.networkInfoToShow.averageDistance = 0;
    this->state.networkInfoToShow.diameter = 0;
    this->state.networkInfoToShow.componentCount = 0;
    this->state.networkInfoToShow.maximumComponentSize = 0;

    update_network_informations();
}

void NetworkWizardApplication::update_network_informations()
{
    GObject *tbVertexCount = gtk_builder_get_object(uiBuilder, "tbVertexCount");
    GObject *tbEdgeCount = gtk_builder_get_object(uiBuilder, "tbEdgeCount");
    GObject *tbAvgDeg = gtk_builder_get_object(uiBuilder, "tbAvgDegree");
    GObject *tbAvgClustCoef = gtk_builder_get_object(uiBuilder, "tbAvgClusturingCoeff");
    GObject *tbAvgDist = gtk_builder_get_object(uiBuilder, "tbAvgDistance");
    GObject *tbDiam = gtk_builder_get_object(uiBuilder, "tbDiameter");
    GObject *tbCompCount = gtk_builder_get_object(uiBuilder, "tbComponentCount");
    GObject *tbMaxComp = gtk_builder_get_object(uiBuilder, "tbMaxComponentSize");

    gtk_entry_set_text(GTK_ENTRY(tbVertexCount), std::to_string(state.networkInfoToShow.vertexCount).c_str());
    gtk_entry_set_text(GTK_ENTRY(tbEdgeCount), std::to_string(state.networkInfoToShow.edgeCount).c_str());
    gtk_entry_set_text(GTK_ENTRY(tbAvgDeg), std::to_string(state.networkInfoToShow.averageDegree).c_str());
    gtk_entry_set_text(GTK_ENTRY(tbAvgClustCoef), std::to_string(state.networkInfoToShow.averageClusteringCoefficient).c_str());
    gtk_entry_set_text(GTK_ENTRY(tbAvgDist), std::to_string(state.networkInfoToShow.averageDistance).c_str());
    gtk_entry_set_text(GTK_ENTRY(tbDiam), std::to_string(state.networkInfoToShow.diameter).c_str());
    gtk_entry_set_text(GTK_ENTRY(tbCompCount), std::to_string(state.networkInfoToShow.componentCount).c_str());
    gtk_entry_set_text(GTK_ENTRY(tbMaxComp), std::to_string(state.networkInfoToShow.maximumComponentSize).c_str());
}

GObject *NetworkWizardApplication::get_object_by_id(const gchar *objId) const
{
    GObject *uiElement = gtk_builder_get_object(this->uiBuilder, objId);
    return uiElement;
}

GtkWindow *NetworkWizardApplication::get_main_window() const
{
    return GTK_WINDOW(this->mainWindow);
}

void thread_test()
{
    fprintf(stdout, "What\n");
}

void internal_recalculate_network_stats(NetworkMatrix *network, NetworkInformations *result)
{

    result->vertexCount = network->vertex_count();
    fprintf(stdout, "1\n");
    result->edgeCount = network->edge_count();
    fprintf(stdout, "2\n");
    result->averageDegree = network->get_average_degree();
    fprintf(stdout, "3\n");
    result->averageClusteringCoefficient = network->get_average_clustering_coefficient();
    fprintf(stdout, "4\n");

    auto distanceMatrix = network->get_distance_matrix();
    auto components = network->get_components();
    GraphComponent maxComponent = get_biggest_component(components);

    result->averageDistance = network->get_network_average_distance(distanceMatrix);
    result->diameter = network->get_network_longest_distance(distanceMatrix);
    result->componentCount = components.size();
    result->maximumComponentSize = maxComponent.size();

    fprintf(stdout, "What\n");
}

void NetworkWizardApplication::recalculate_network_stats()
{
    if (state.network.vertex_count() == 0)
    {
        clear_network();
        return;
    }
    NetworkInformations info = {};
    auto af1 = std::async(internal_recalculate_network_stats, &state.network, &info);
    //std::async(thread_test);
    //std::thread(thread_test);
    //state.networkInfoToShow = info;

    update_network_informations();
}