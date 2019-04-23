#include <app_global.h>
#include <app_signals.h>

constexpr char UI_File[] = "/home/mor0146/github/dataAnalysisCourse/MAD_II/ui/draft.ui";

int main(int argc, char *argv[])
{

    NetworkWizardApplication networkWizardApp = {};
    gtk_init(&argc, &argv);

    networkWizardApp.uiBuilder = gtk_builder_new();
    GError *builderError = NULL;
    if (gtk_builder_add_from_file(networkWizardApp.uiBuilder, UI_File, &builderError) == 0)
    {
        g_printerr("Error loading file: %s\n", builderError->message);
        g_clear_error(&builderError);
        return 1;
    }
    else
    {
        fprintf(stdout, "Correctly parsed ui\n");
    }

    attach_signals(networkWizardApp);
    networkWizardApp.show_app_window("windowMainWindow");

    gtk_main();

    fprintf(stdout, "Closed the application\n");
    return 0;
}
