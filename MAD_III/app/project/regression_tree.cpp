#include "regression_tree.h"

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

    // Calculate MSE.
    sum = 0.0;
    for (const auto tId : transactionIds)
    {
        sum += pow((df(tId, targetColumn) - predictedValue), 2);
    }
    mse = sum / static_cast<double>(transactionIds.size());

    always_assert(!isnan(mse));
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

TreeNodeCandidate RegressionTreeBuilder::find_best_split_for_attribute(const TreeNode &currentNode, const size_t attributeIndex)
{
    using namespace azgra::collection;

    const size_t nodeTransactionCount = currentNode.transactionIds.size();
    always_assert(nodeTransactionCount >= m_minSamplesSplit && "Node doesn't have enough samples for split");


    // NOTE(Moravec):   At first we will try the split on all values.
    //                  In case this solution is slow we will use every tenth percentile.

    const auto columnTrainValues = m_df.get_attribute_values_for_transactions(attributeIndex, currentNode.transactionIds);

    const auto uniqueSplitValues = distinct(columnTrainValues.begin(), columnTrainValues.end());
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
    if (childHeight > m_maxHeight)
    {
        m_maxHeight = childHeight;
        //fprintf(stdout, "New max height: %lu\n", m_maxHeight);
    }

    // All the candidates
    std::vector<TreeNodeCandidate> candidates(numberOfPossibleSplits);
    for (size_t i = 0; i < numberOfPossibleSplits; ++i)
    {
        const double splitValue = uniqueSplitValues[i];

        TreeNodeCandidate candidate = TreeNodeCandidate(attributeIndex, splitValue);

        candidate.lChild = std::make_unique<TreeNode>(childHeight);
        candidate.rChild = std::make_unique<TreeNode>(childHeight);

        std::vector<size_t> lChildTransactions = where(currentNode.transactionIds.begin(),
                                                       currentNode.transactionIds.end(),
                                                       [this, attributeIndex, splitValue](const size_t tId)
                                                       {
                                                           return (m_df(tId, attributeIndex) <= splitValue);
                                                       });
        // rChild gets all the remaining transactions.
        std::vector<size_t> rChildTransactions = except(currentNode.transactionIds.begin(),
                                                        currentNode.transactionIds.end(),
                                                        lChildTransactions.begin(),
                                                        lChildTransactions.end());


        candidate.lChild->transactionIds = std::move(lChildTransactions);
        candidate.rChild->transactionIds = std::move(rChildTransactions);

        candidate.lChild->calculate_predicted_value(m_df);
        candidate.rChild->calculate_predicted_value(m_df);

        candidate.mse = (candidate.lChild->mse + candidate.rChild->mse) / 2.0;
        candidate.lChildSize = candidate.lChild->transactionIds.size();
        candidate.rChildSize = candidate.rChild->transactionIds.size();

        candidates[i] = std::move(candidate);
    }

    // Select the best candidate, the lowest MSE.
    TreeNodeCandidate bestCandidate = std::move(*(std::min_element(candidates.begin(),
                                                                   candidates.end(),
                                                                   [](const TreeNodeCandidate &a, const TreeNodeCandidate &b)
                                                                   {
                                                                       return (a.mse < b.mse);
                                                                   })));

    always_assert(!isnan(bestCandidate.predictedValue));

    return bestCandidate;
}

void RegressionTreeBuilder::create_best_split(TreeNode &node)
{
    const size_t attributeCount = m_df.get_attribute_count();
    const size_t targetAttributeIndex = m_df.get_target_attribute_index();
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
        if (attributeIndex == targetAttributeIndex)
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
    TreeNodeCandidate bestCandidate = std::move(*(std::min_element(possibleSplits.begin(),
                                                                   possibleSplits.end(),
                                                                   [](const TreeNodeCandidate &a, const TreeNodeCandidate &b)
                                                                   {
                                                                       return (a.mse < b.mse);
                                                                   })));

    always_assert(bestCandidate.splitAttributeIndex < attributeCount &&
                  bestCandidate.splitAttributeIndex != targetAttributeIndex);

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
