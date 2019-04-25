#pragma once
#include <app_global.h>
#include <app_signals.h>
#include <thread>
#include <future>

void internal_recalculate_network_stats(NetworkMatrix *network, NetworkInformations *result);
void thread_test();

class NetworkWizardApplication
{
private:
    GtkBuilder *uiBuilder;
    GObject *mainWindow;

    void update_network_informations();

public:
    ApplicationState state;
    NetworkWizardApplication(GtkBuilder *builder, const char *mainWindowId);

    GObject *get_object_by_id(const gchar *objId) const;
    GtkWindow *get_main_window() const;

    void clear_network();
    void recalculate_network_stats();
};