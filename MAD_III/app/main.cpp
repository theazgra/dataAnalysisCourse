// #include "tutorials/tutorial_1.h"
#include "tutorials/tutorial_2.h"
#include "decision_tree/loading.h"
#include "decision_tree/DecisionTree.h"

static void print_confusion_matrix(const azgra::Matrix<azgra::u32> &mat)
{
    std::stringstream ss;
    for (size_t row = 0; row < mat.rows(); ++row)
    {
        for (size_t col = 0; col < mat.cols(); ++col)
        {
            ss << mat.at(row, col) << " ";
        }
        ss << '\n';
    }
    ss << '\n';
    fprintf(stdout, "%s", ss.str().c_str());
}


int main(int, char **)
{
    // TODO: Rozdeleni na trenovaci xa testovaci dataset + Confusion Matrix
    using namespace DecisionTree;
    using namespace azgra::collection;

    auto categoricalTicTacToe = load_categorial("/mnt/d/codes/git/dataAnalysisCourse/MAD_III/data/tic-tac-toe.csv", ';', 9);
    fprintf(stdout, "Loaded %lu categorical transactions with %lu classes  and %lu categories\n",
            categoricalTicTacToe.transactions.size(), categoricalTicTacToe.classes.size(), categoricalTicTacToe.categories.size());

    auto kFoldDatasetTTT = create_cross_validation_datasets(categoricalTicTacToe, 5);

    for (size_t i = 0; i < kFoldDatasetTTT.size(); ++i)
    {
        fprintf(stdout, "Train dataset size: %lu\nTest dataset size: %lu\n",
                kFoldDatasetTTT[i].first.transactions.size(), kFoldDatasetTTT[i].second.transactions.size());
        TreeBuilder numericalBuilder(kFoldDatasetTTT[i].first, 2);
        numericalBuilder.build(10, 15);

        ClassificationResult classification = numericalBuilder.test_classification(kFoldDatasetTTT[i].second);
        fprintf(stdout, "Categorical classification precision on test dataset: %.5f\n", classification.precision);
        print_confusion_matrix(classification.confusionMatrix);
    }

//    auto trainTest = split_dataset(categoricalTicTacToe, 0.2);
//    fprintf(stdout, "Train dataset size: %lu\nTest dataset size: %lu\n",
//            trainTest.first.transactions.size(), trainTest.second.transactions.size());
//
//    TreeBuilder builder(trainTest.first, 2);
//    builder.build(20, 15);
//
//    double prec = builder.test_classification(trainTest.second);
//    fprintf(stdout, "Categorical classification precision on test dataset: %.5f\n", prec);

    auto numericalIris = load_numerical("/mnt/d/codes/git/dataAnalysisCourse/MAD_III/data/iris.csv", ';', 4);
    fprintf(stdout, "Loaded %lu numerical transactions with %lu classes\n",
            numericalIris.transactions.size(), numericalIris.classes.size());

    auto trainTestIris = split_dataset(numericalIris, 0.2);
    auto kFoldDataset = create_cross_validation_datasets(numericalIris, 5);
    fprintf(stdout, "Train dataset size: %lu\nTest dataset size: %lu\n",
            trainTestIris.first.transactions.size(), trainTestIris.second.transactions.size());

    for (size_t i = 0; i < kFoldDataset.size(); ++i)
    {
        fprintf(stdout, "Train dataset size: %lu\nTest dataset size: %lu\n",
                kFoldDataset[i].first.transactions.size(), kFoldDataset[i].second.transactions.size());
        TreeBuilder numericalBuilder(kFoldDataset[i].first, 2);
        numericalBuilder.build(10, 15);

        ClassificationResult classification = numericalBuilder.test_classification(kFoldDataset[i].second);
        fprintf(stdout, "Numerical classification precision on test dataset: %.5f\n", classification.precision);
        print_confusion_matrix(classification.confusionMatrix);
    }

    return 0;
}