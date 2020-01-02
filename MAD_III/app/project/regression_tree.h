#pragma once

#include "dataframe.h"
#include <azgra/collection/enumerable.h>
#include <memory>


//struct ClassificationResult
//{
//    double precision;
//    azgra::Matrix<azgra::u32> confusionMatrix;
//};

struct TreeNode
{
    size_t splitAttributeIndex{};
    //double splitValueLowerBound{};
    double splitValueUpperBound{};
    double mse = -1.0;

    std::vector<size_t> transactionIds;

    std::unique_ptr<TreeNode> lChild;
    std::unique_ptr<TreeNode> rChild;

    size_t height = 1;
    double predictedValue{};

    TreeNode() = default;

    //TreeNode(const TreeNode &) = delete;

    explicit TreeNode(std::vector<size_t> &transactions)
    {
        transactionIds = std::move(transactions);
    }

    explicit TreeNode(const size_t nodeHeight) : height(nodeHeight)
    {
    }

    /**
     * Calculate predicated value as a mean of target value. Also calculate MSE.
     * @param df DataFrame.
     */
    void calculate_predicted_value(const DataFrame &df);

    [[nodiscard]] double predict(const DataFrame &df, size_t rowIndex) const;

};

struct TreeNodeCandidate : TreeNode
{
    size_t splitAttributeIndex{};

    TreeNodeCandidate() = default;

    explicit TreeNodeCandidate(const size_t attributeIndex, const double splitValue) : splitAttributeIndex(attributeIndex)
    {
        splitValueUpperBound = splitValue;
    }
};

class RegressionTree
{
    friend class RegressionTreeBuilder;

private:
    /**
     * Current height of the regression tree.
     */
    size_t m_currentHeight = 0;

    TreeNode m_root;

public:
    RegressionTree() = default;

    [[nodiscard]] double predict(const DataFrame &df, size_t rowIndex) const;

//    ClassificationResult test_classification(Dataset <T> &testDataset)
//    {
//        const auto classes = testDataset.classes;
//        const size_t classCount = classes.size();
//        std::map<int, int> classMap;
//        int index = 0;
//        for (const auto &c : classes)
//        {
//            classMap[c] = index++;
//        }
//
//        ClassificationResult result = {};
//        result.confusionMatrix = azgra::Matrix<azgra::u32>(classCount, classCount, 0);
//
//
//        size_t correct = 0;
//        for (Transaction <T> &t : testDataset.transactions)
//        {
//            t.guessedClassIndex = classify(t);
//            result.confusionMatrix.at(classMap[t.classIndex], classMap[t.guessedClassIndex]) += 1;
//            if (t.guessedClassIndex == t.classIndex)
//            {
//                ++correct;
//            }
//        }
//        result.precision = static_cast<double> (correct) / static_cast<double   >(testDataset.transactions.size());
//        return result;
//    }

};


class RegressionTreeBuilder
{
private:
    /**
     * The maximum height of the tree.
     */
    size_t m_maxTreeHeight = 9999;

    /**
     * The minimum number of samples required to split an internal node.
     */
    size_t m_minSamplesSplit = 2;

    /**
     * Dataset used for training.
     */
    DataFrame m_df;

    /**
     * Train row indices into m_df.
     */
    std::vector<size_t> m_trainIndices;

    /**
     * Find the best value for the numeric split of selected attribute.
     * @param currentNode Current node, parent to the new split node.
     * @param attributeIndex Selected attribute.
     * @return Best attribute slit.
     */
    [[nodiscard]] TreeNodeCandidate find_best_split_for_attribute(const TreeNode &currentNode, const size_t attributeIndex) const;

    void create_best_split(TreeNode &node) const;

public:

    explicit RegressionTreeBuilder(DataFrame &df,
                                   const std::vector<size_t> &trainIndices,
                                   size_t minSamplesSplit = 2,
                                   size_t maxTreeHeight = 9999);

    [[nodiscard]] RegressionTree build() const;

};
