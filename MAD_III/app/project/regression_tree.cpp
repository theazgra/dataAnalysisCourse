#include "regression_tree.h"

constexpr int MAX_NUMBER_OF_SPLITS = 10;

void TreeNode::calculate_predicted_value(const DataFrame &df)
{
    // Punish empty node with maximum MSE.
    if (transactionIds.empty())
    {
        predictedValue = NAN;
        mse = std::numeric_limits<double>::max();
        return;
    }
    const size_t targetColumn = df.get_target_attribute_index();
    double sum = 0.0;
    for (const auto tId : transactionIds)
    {
        sum += df(tId, targetColumn);
    }
    predictedValue = sum / static_cast<double>(transactionIds.size());
    always_assert(!isnan(predictedValue));
}

double TreeNode::predict(const DataFrame &df, const size_t rowIndex) const
{
    const double value = df(rowIndex, splitAttributeIndex);
//    fprintf(stdout, "Node height: %lu; PredictedValue: %.5f; Attribute: %lu SplitValue: %.5f; AttributeValue: %.5f\n",
//            height,
//            predictedValue,
//            splitAttributeIndex,
//            splitValueUpperBound, value);

    if (!lChild && !rChild)
    {
        return predictedValue;
    }
    always_assert((lChild && rChild) && "Some child is not initialized.");

    if (value <= splitValueUpperBound)
    {
        return lChild->predict(df, rowIndex);
    }
    else
    {
        return rChild->predict(df, rowIndex);
    }
}


double RegressionTree::predict(const DataFrame &df, const size_t rowIndex) const
{
    return m_root.predict(df, rowIndex);
}

RegressionResult RegressionTree::test_prediction(const DataFrame &df, const std::vector<size_t> &tIds, const bool verbose) const
{

    const size_t testSize = tIds.size();
    const size_t targetAttributeIndex = df.get_target_attribute_index();

    RegressionResult rr = {};

    double trueValue, predictedValue, error;
    for (const size_t tId : tIds)
    {
        trueValue = df(tId, targetAttributeIndex);
        predictedValue = predict(df, tId);

        if (verbose)
            fprintf(stdout, "== T: %.4f P: %.4f\n", trueValue, predictedValue);

        error = predictedValue - trueValue;
        rr.mse += pow(error, 2);
        rr.mae += abs(error);
        rr.maxError = std::max(rr.maxError, abs(error));
    }

    rr.mse /= static_cast<double>(testSize);
    rr.mae /= static_cast<double>(testSize);

    return rr;
}

RegressionTreeBuilder::RegressionTreeBuilder(DataFrame &df,
                                             const std::vector<size_t> &trainIndices,
                                             size_t minSamplesSplit,
                                             size_t maxTreeHeight)
{
    if (df.size() <= 0)
    {
        throw std::runtime_error("Empty DataFrame");
    }


    m_df = std::move(df);
    m_targetAttributeIndex = m_df.get_target_attribute_index();
    m_trainIndices = trainIndices;
    m_minSamplesSplit = minSamplesSplit;
    m_maxTreeHeight = maxTreeHeight;
}

RegressionTree RegressionTreeBuilder::build()
{
    m_nodeCount = 1;
    RegressionTree tree;
    tree.m_currentHeight = 1;

    auto transactionIds = m_trainIndices;
    tree.m_root = TreeNode(transactionIds);

    create_best_split(tree.m_root);

    tree.m_currentHeight = m_maxHeight;

    fprintf(stdout, "Built regression tree of height %lu with %lu nodes\n", tree.m_currentHeight, m_nodeCount);

    return tree;
}

void RegressionTreeBuilder::calculate_candidate_mse(TreeNodeCandidate &candidate, const std::vector<size_t> &tIds)
{
    double mse = 0.0;
    double originalValue, predictedValue;
    for (const auto tId : tIds)
    {
        originalValue = m_df(tId, m_targetAttributeIndex);

        predictedValue = (originalValue <= candidate.splitValueUpperBound) ?
                         candidate.lChild->predictedValue : candidate.rChild->predictedValue;

        mse += pow((originalValue - predictedValue), 2);
    }
    mse /= static_cast<double>(tIds.size());
    candidate.mse = mse;
}

TreeNodeCandidate RegressionTreeBuilder::find_best_split_for_attribute(const TreeNode &currentNode, const size_t attributeIndex)
{
    using namespace azgra::collection;

    const size_t nodeTransactionCount = currentNode.transactionIds.size();
    always_assert(nodeTransactionCount >= m_minSamplesSplit && "Node doesn't have enough samples for split");


    // NOTE(Moravec):   At first we will try the split on all values.
    //                  In case this solution is slow we will use every tenth percentile.

    const auto columnTrainValues = m_df.get_attribute_values_for_transactions(attributeIndex, currentNode.transactionIds);

    auto uniqueSplitValues = distinct(columnTrainValues.begin(), columnTrainValues.end());
    const size_t numberOfPossibleSplits = uniqueSplitValues.size();
    if (numberOfPossibleSplits == 1)
    {
        // The node is pure.
        TreeNodeCandidate pureCandidate = TreeNodeCandidate(attributeIndex, uniqueSplitValues[0]);
        pureCandidate.isPure = true;
        pureCandidate.mse = std::numeric_limits<double>::max();
        return pureCandidate;
    }

    const size_t childHeight = currentNode.height + 1;
    m_maxHeight = std::max(m_maxHeight, childHeight);


    // Reduce the number of possible split count to MAX_NUMBER_OF_SPLITS
    std::sort(uniqueSplitValues.begin(), uniqueSplitValues.end());
    std::array<double, MAX_NUMBER_OF_SPLITS> splitValues{};
    const size_t splitCount = (numberOfPossibleSplits > MAX_NUMBER_OF_SPLITS) ? MAX_NUMBER_OF_SPLITS : numberOfPossibleSplits;

    for (size_t j = 0; j < splitCount; ++j)
    {
        if (numberOfPossibleSplits <= MAX_NUMBER_OF_SPLITS)
        {
            splitValues[j] = uniqueSplitValues[j];
        }
        else
        {
            // every tenth percentile
            const auto srcIndex = static_cast<size_t>(0.1 + (0.1 * static_cast<float>(j)) * static_cast<float>(numberOfPossibleSplits));
            splitValues[j] = uniqueSplitValues[srcIndex];
        }
    }

    // All the candidates
    std::vector<TreeNodeCandidate> candidates;
    for (size_t i = 0; i < splitCount; ++i)
    {
        const double splitValue = splitValues[i];

        std::vector<size_t> lChildTransactions = where(currentNode.transactionIds.begin(),
                                                       currentNode.transactionIds.end(),
                                                       [this, attributeIndex, splitValue](const size_t tId)
                                                       {
                                                           return (m_df(tId, attributeIndex) <= splitValue);
                                                       });

        if (lChildTransactions.empty())
            continue;

        // rChild gets all the remaining transactions.
        std::vector<size_t> rChildTransactions = except(currentNode.transactionIds.begin(),
                                                        currentNode.transactionIds.end(),
                                                        lChildTransactions.begin(),
                                                        lChildTransactions.end());

        if (rChildTransactions.empty())
            continue;

        TreeNodeCandidate candidate = TreeNodeCandidate(attributeIndex, splitValue);
        candidate.lChild = std::make_unique<TreeNode>(childHeight);
        candidate.rChild = std::make_unique<TreeNode>(childHeight);

        candidate.lChild->transactionIds = std::move(lChildTransactions);
        candidate.rChild->transactionIds = std::move(rChildTransactions);

        candidate.lChild->calculate_predicted_value(m_df);
        candidate.rChild->calculate_predicted_value(m_df);

        calculate_candidate_mse(candidate, currentNode.transactionIds);
        candidate.lChildSize = candidate.lChild->transactionIds.size();
        candidate.rChildSize = candidate.rChild->transactionIds.size();

        candidates.emplace_back(std::move(candidate));
    }
    always_assert(!candidates.empty() && "No valid candidates for attribute.");

    // Select the best candidate, the lowest MSE.
    TreeNodeCandidate bestCandidate = std::move(candidates[0]);
    for (size_t i = 1; i < candidates.size(); ++i)
    {
        if (candidates[i].mse < bestCandidate.mse)
        {
            bestCandidate = std::move(candidates[i]);
        }
    }

    always_assert(!isnan(bestCandidate.predictedValue));
    always_assert(bestCandidate.mse != -1.0);

    return bestCandidate;
}

void RegressionTreeBuilder::create_best_split(TreeNode &node)
{
    const size_t attributeCount = m_df.get_attribute_count();
    const size_t nodeSampleCount = node.transactionIds.size();
    // Stop criteria for splitting.
    if ((node.height > m_maxTreeHeight) || (nodeSampleCount < m_minSamplesSplit))
    {
        return;
    }


    // We want to test split on all attributes except the target attribute.
    std::vector<TreeNodeCandidate> possibleSplits;
    possibleSplits.reserve(attributeCount - 1);

    for (size_t attributeIndex = 0; attributeIndex < attributeCount; ++attributeIndex)
    {
        if (attributeIndex == m_targetAttributeIndex)
        { continue; }

        possibleSplits.push_back(find_best_split_for_attribute(node, attributeIndex));
    }

    const bool allChildrenArePure = azgra::collection::all(possibleSplits.begin(), possibleSplits.end(), [](const TreeNodeCandidate &a)
    {
        return a.isPure;
    });

    if (allChildrenArePure)
    {
        return;
    }

    // Select the best candidate, the lowest MSE.
    TreeNodeCandidate bestCandidate = std::move(possibleSplits[0]);
    for (size_t i = 1; i < possibleSplits.size(); ++i)
    {
        if (possibleSplits[i].mse < bestCandidate.mse)
        {
            bestCandidate = std::move(possibleSplits[i]);
        }
    }

    always_assert((bestCandidate.rChild && bestCandidate.lChild) && "Best candidate doesn't have children!");

    always_assert(bestCandidate.splitAttributeIndex < attributeCount &&
                  bestCandidate.splitAttributeIndex != m_targetAttributeIndex);

    node.splitAttributeIndex = bestCandidate.splitAttributeIndex;
    node.splitValueUpperBound = bestCandidate.splitValueUpperBound;
    node.lChild = std::move(bestCandidate.lChild);
    node.rChild = std::move(bestCandidate.rChild);

    m_nodeCount += 2;

    create_best_split(*node.lChild);
    create_best_split(*node.rChild);
}

DataFrame &RegressionTreeBuilder::df()
{
    return m_df;
}
