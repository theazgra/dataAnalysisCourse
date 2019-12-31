#include "project/ecsv_loader.h"
#include "project/regression_tree.h"

int main(int, char **)
{
    // TODO: 1. Check split only on unique values, there will be duplicates!
    DataFrame df;
    {
        const ECsv e_csv = ECsvLoader::load_ecsv_file("/mnt/d/codes/git/dataAnalysisCourse/MAD_III/data/car_import.csv", true);
        df = std::move(e_csv.convert_to_dataframe());
    }
    df.print_header();
    df.min_max_scaling(0.0, 1.0);
    //azgra::io::save_matrix_to_csv(df.get_data(), ',', "normalized_matrix.csv");


    RegressionTreeBuilder RT_builder(df);
    RegressionTree rt = RT_builder.build();

    return 0;
}