#include "project/ecsv_loader.h"
#include "project/regression_tree.h"
#include <azgra/io/text_file_functions.h>

void test_regression_tree(DataFrame &df,
                          const std::pair<std::vector<size_t>, std::vector<size_t>> &indices,
                          const size_t minSampleLeaf)
{
    // builder move the dataframe
    RegressionTreeBuilder rtBuilder(df, indices.first, minSampleLeaf, 1000);
    RegressionTree rt = rtBuilder.build();
    df = std::move(rtBuilder.df());

    const auto regressionScore = rt.test_prediction(df, indices.second);

    fprintf(stdout, "=== Regression tree ===\n MinSamplesLeaf = %lu\n  MSE: %.5f\n  MAE: %.5f\n  MaxE: %.5f\n  R2: %.5f\n",
            minSampleLeaf,
            regressionScore.mse,
            regressionScore.mae,
            regressionScore.maxError,
            regressionScore.r2);

}


int main(int argc, char **argv)
{
    char *dfFile = const_cast<char *> ("/mnt/d/codes/git/dataAnalysisCourse/MAD_III/data/car_import2.csv");
    float sampleSize = 1.0;
    if (argc > 1)
    {
        dfFile = argv[1];
        sampleSize = 0.15;
    }
    fprintf(stdout, "Building regression tree for df: %s\n", dfFile);

    DataFrame df;
    const ECsv e_csv = ECsvLoader::load_ecsv_file(dfFile, false);
    df = e_csv.convert_to_dataframe();

    df.print_header();
//    df.min_max_scaling(0.0, 1.0);

    azgra::io::save_matrix_to_csv(df.matrix(), ';', "df.csv");


    const auto indices = df.get_train_test_indices(sampleSize, 0.8, true);
    fprintf(stdout, "Train DF size: %lu\nTest DF size: %lu\n", indices.first.size(), indices.second.size());

    test_regression_tree(df, indices, 15);
    test_regression_tree(df, indices, 10);
    test_regression_tree(df, indices, 5);
    test_regression_tree(df, indices, 2);

    return 0;
}