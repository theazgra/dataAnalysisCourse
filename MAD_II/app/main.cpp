#include <network_wizard_app.h>

constexpr char UI_File[] = "/home/mor0146/github/dataAnalysisCourse/MAD_II/app/ui_files/draft.ui";

int main(int argc, char *argv[])
{

    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new();
    GError *builderError = NULL;
    if (gtk_builder_add_from_file(builder, UI_File, &builderError) == 0)
    {
        g_printerr("Error loading file: %s\n", builderError->message);
        g_clear_error(&builderError);
        return 1;
    }
    else
    {
        fprintf(stdout, "Correctly parsed ui\n");
    }

    NetworkWizardApplication networkWizardApp(builder, "windowMainWindow");

    gtk_main();

    fprintf(stdout, "Closed the application\n");
    return 0;
}
