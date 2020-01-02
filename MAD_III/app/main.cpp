#include "project/ecsv_loader.h"
#include "project/regression_tree.h"

int main(int, char **)
{
    DataFrame df;
    {
        const ECsv e_csv = ECsvLoader::load_ecsv_file("/mnt/d/codes/git/dataAnalysisCourse/MAD_III/data/car_import.csv", true);
        df = e_csv.convert_to_dataframe();
    }

    df.print_header();
    df.min_max_scaling(0.0, 1.0);

    const auto[trainDF, testDF] = df.get_train_test_indices(0.8, true);

    RegressionTreeBuilder rtBuilder(df, trainDF);
    RegressionTree rt = rtBuilder.build();


    [[maybe_unused]] const size_t r = df.matrix().rows();

    return 0;
}