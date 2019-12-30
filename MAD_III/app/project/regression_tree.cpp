#include "regression_tree.h"

void TreeNode::calculate_predicted_value(const DataFrame &df)
{
    const size_t targetColumn = df.get_target_attribute_index();
    double sum = 0.0;
    for (const auto tId : transactionIds)
    {
        sum += df(tId, targetColumn);
    }
    predictedValue = sum / static_cast<double>(transactionIds.size());

    // Calculate MSE.
    sum = 0.0;
    for (const auto tId : transactionIds)
    {
        sum += pow((df(tId, targetColumn) - predictedValue), 2);
    }
    mse = sum / static_cast<double>(transactionIds.size());
}

double TreeNode::predict(const DataFrame &df, const size_t rowIndex) const
{
    if (!lChild && !rChild)
    {
        return predictedValue;
    }
    always_assert((lChild && rChild) && "Some child is not initialized.");

    const double value = df(rowIndex, splitAttributeIndex);
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

RegressionTreeBuilder::RegressionTreeBuilder(const DataFrame &df, size_t minSamplesSplit, size_t maxTreeHeight)
{
    if (df.size() <= 0)
    {
        throw std::runtime_error("Empty DataFrame");
    }

    m_trainDataset = df;
    m_minSamplesSplit = minSamplesSplit;
    m_maxTreeHeight = maxTreeHeight;
}

RegressionTree RegressionTreeBuilder::build() const
{
    RegressionTree tree;
    tree.m_currentHeight = 1;

    auto transactionIds = azgra::collection::Enumerable<size_t>::range(0, m_trainDataset.size()).to_vector();
    tree.m_root = TreeNode(transactionIds);

    build_tree(tree.m_root);

    fprintf(stdout, "Built regression tree of height %lu\n", tree.m_currentHeight);

    return tree;
}


void RegressionTreeBuilder::build_tree(TreeNode &node) const
{
    //create_best_split(node, f);
}

TreeNodeCandidate RegressionTreeBuilder::find_best_split_for_attribute(const TreeNode &currentNode, const size_t attributeIndex) const
{
    using namespace azgra::collection;

    const size_t nodeTransactionCount = currentNode.transactionIds.size();
    always_assert(nodeTransactionCount >= m_minSamplesSplit && "Node doesn't have enough samples for split");


    // NOTE(Moravec):   At first we will try the split on all values.
    //                  In case this solution is slow we will use every tenth percentile.

    // All the candidates
    std::vector<TreeNodeCandidate> candidates(nodeTransactionCount);
    for (size_t i = 0; i < nodeTransactionCount; ++i)
    {
        const double splitValue = m_trainDataset(currentNode.transactionIds[i], attributeIndex);

        TreeNodeCandidate candidate = TreeNodeCandidate(attributeIndex, splitValue);

        candidate.lChild = std::make_unique<TreeNode>(currentNode.height + 1);
        candidate.rChild = std::make_unique<TreeNode>(currentNode.height + 1);

        std::vector<size_t> lChildTransactions = where(currentNode.transactionIds.begin(),
                                                       currentNode.transactionIds.end(),
                                                       [this, attributeIndex, splitValue](const size_t tId)
                                                       {
                                                           return (m_trainDataset(tId, attributeIndex) <= splitValue);
                                                       });
        // rChild gets all the remaining transactions.
        std::vector<size_t> rChildTransactions = except(currentNode.transactionIds.begin(),
                                                        currentNode.transactionIds.end(),
                                                        lChildTransactions.begin(),
                                                        lChildTransactions.end());


        candidate.lChild->transactionIds = std::move(lChildTransactions);
        candidate.rChild->transactionIds = std::move(rChildTransactions);

        candidate.lChild->calculate_predicted_value(m_trainDataset);
        candidate.rChild->calculate_predicted_value(m_trainDataset);

        candidate.mse = (candidate.lChild->mse + candidate.rChild->mse) / 2.0;

        candidates[i] = std::move(candidate);
    }

    // Select the best candidate, the lowest MSE.
    TreeNodeCandidate bestCandidate = std::move(*(std::min_element(candidates.begin(),
                                                                   candidates.end(),
                                                                   [](const TreeNodeCandidate &a, const TreeNodeCandidate &b)
                                                                   {
                                                                       return (a.mse < b.mse);
                                                                   })));

    return bestCandidate;
}

void RegressionTreeBuilder::create_best_split(TreeNode &node) const
{
    const size_t attributeCount = m_trainDataset.get_attribute_count();
    const size_t targetAttributeIndex = m_trainDataset.get_target_attribute_index();
    const size_t nodeTransactionCount = node.transactionIds.size();
    // Stop criteria for splitting.
    if ((node.height > m_maxTreeHeight) || (nodeTransactionCount < m_minSamplesSplit))
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
    node.lChild = std::move(bestCandidate.lChild);
    node.rChild = std::move(bestCandidate.rChild);

    create_best_split(*node.lChild);
    create_best_split(*node.rChild);
}