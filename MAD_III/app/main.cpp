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

void test_regression_tree_n(DataFrame &df,
                            const std::pair<std::vector<size_t>, std::vector<size_t>> &indices,
                            const size_t minSampleLeaf,
                            const size_t n)
{
    using namespace azgra::collection;
    std::vector<RegressionResult> rrs(n);

    double bestR2 = -100.0;
    for (size_t i = 0; i < n; ++i)
    {
        RegressionTreeBuilder rtBuilder(df, indices.first, minSampleLeaf, 1000);
        RegressionTree rt = rtBuilder.build();
        df = std::move(rtBuilder.df());
        rrs[i] = rt.test_prediction(df, indices.second);
        bestR2 = std::max(bestR2, rrs[i].r2);
    }

    const double meanMAE = sum(rrs.begin(), rrs.end(), [](const RegressionResult &rr)
    { return rr.mae; }, 0.0) / static_cast<double>(n);
    const double meanMSE = sum(rrs.begin(), rrs.end(), [](const RegressionResult &rr)
    { return rr.mse; }, 0.0) / static_cast<double>(n);
    const double meanR2 = sum(rrs.begin(), rrs.end(), [](const RegressionResult &rr)
    { return rr.r2; }, 0.0) / static_cast<double>(n);




    for (const auto &rr : rrs)
    {
        fprintf(stdout, "R2: %.5f\n", rr.r2);
    }

    fprintf(stdout, "=== Regression tree ===\n Mean MinSamplesLeaf = %lu\n  Mean results of %lu runs\n  MSE: %.5f\n  MAE: %.5f\n  "
                    "R2: %.5f\n",
            minSampleLeaf,
            n,
            meanMSE,
            meanMAE,
            meanR2);

    fprintf(stdout, "Best R2: %.5f\n", bestR2);
}


int main(int argc, char **argv)
{
    char *dfFile = const_cast<char *> ("/mnt/d/codes/git/dataAnalysisCourse/MAD_III/data/car_import_shuffled.csv");
    float sampleSize = 1.0;
//    char *dfFile = const_cast<char *> ("/mnt/d/codes/git/dataAnalysisCourse/MAD_III/data/beijing_pm25_ecsv.csv");
//    float sampleSize = 0.3;
    if (argc > 1)
    {
        dfFile = argv[1];
        sampleSize = 0.3;
    }

    fprintf(stdout, "Building regression tree for df: %s\n", dfFile);

    DataFrame df;
    const ECsv e_csv = ECsvLoader::load_ecsv_file(dfFile, true);
    df = e_csv.convert_to_dataframe();

    df.min_max_scaling(0.0, 1.0);

    azgra::io::save_matrix_to_csv(df.matrix(), ';', "df.csv");

    const auto indices = df.get_train_test_indices(sampleSize, 0.8, false);
    fprintf(stdout, "Train DF size: %lu\nTest DF size: %lu\n", indices.first.size(), indices.second.size());

    test_regression_tree(df, indices, 10);
    test_regression_tree(df, indices, 5);
    test_regression_tree(df, indices, 2);
//    test_regression_tree_n(df, indices, 2, 20);


    return 0;
}