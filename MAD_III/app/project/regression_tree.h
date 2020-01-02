#pragma once

#include "dataframe.h"
#include <azgra/collection/enumerable.h>
#include <array>
#include <memory>
#include <omp.h>

struct RegressionResult
{
    double mse = 0.0;
    double mae = 0.0;
    double maxError = 0.0;
    double r2 = -1.0;
};

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
     * Calculate predicated value as a mean of target values
     * @param df DataFrame.
     */
    void calculate_predicted_value(const DataFrame &df);

    [[nodiscard]] double predict(const DataFrame &df, size_t rowIndex) const;

};

struct TreeNodeCandidate : TreeNode
{
    size_t splitAttributeIndex{};
    size_t lChildSize{};
    size_t rChildSize{};
    bool isPure = false;

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

    [[nodiscard]] RegressionResult test_prediction(const DataFrame &df, const std::vector<size_t> &tIds, bool verbose = false) const;
};


class RegressionTreeBuilder
{
private:
    /**
     * Maximum number of tested splits on attribute.
     */

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
     * Current maximum height of the tree.
     */
    size_t m_maxHeight = 0;

    /**
     * Current node count in the tree.
     */
    size_t m_nodeCount = 0;

    /**
     * Index of the target attribute.
     */
    size_t m_targetAttributeIndex;

    /**
     * Find the best value for the numeric split of selected attribute.
     * @param currentNode Current node, parent to the new split node.
     * @param attributeIndex Selected attribute.
     * @return Best attribute slit.
     */
    [[nodiscard]] TreeNodeCandidate find_best_split_for_attribute(const TreeNode &currentNode, const size_t attributeIndex);

    void calculate_candidate_mse(TreeNodeCandidate &candidate, const std::vector<size_t> &tIds);
    [[nodiscard]] double calculate_predicted_value(const std::vector<size_t> &tIds);

    [[nodiscard]] double calculate_split_mse(double splitValue,
                                             double lChildValue,
                                             double rChildValue,
                                             const std::vector<size_t> &tIds);

    void create_best_split(TreeNode &node);

public:

    explicit RegressionTreeBuilder(DataFrame &df,
                                   const std::vector<size_t> &trainIndices,
                                   size_t minSamplesSplit = 2,
                                   size_t maxTreeHeight = 9999);

    [[nodiscard]] RegressionTree build();

    DataFrame &df();

};
