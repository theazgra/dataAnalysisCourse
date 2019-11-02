#pragma once

#include <azgra/azgra.h>
#include <type_traits>
#include <azgra/collection/enumerable.h>
#include "dataset.h"
#include <map>

namespace DecisionTree
{
    using namespace azgra::collection;

    enum SplitType
    {
        SplitType_Categorical,
        SplitType_Numerical
    };

    template<typename T>
    struct SplitNode
    {
        size_t attributeIndex;
        T attributeValue = T();
        SplitType nodeType = SplitType::SplitType_Categorical;
        azgra::collection::Enumerable<size_t> transactionIds;
        std::vector<SplitNode<T>> children;
        size_t height = 1;
        int predicatedClass = -1;

        bool attributeIndexSet = false;

        SplitNode() = default;

        void set_attribute_index(const size_t attrib)
        {
            attributeIndexSet = true;
            attributeIndex = attrib;
        }

        static SplitNode categorical()
        {
            SplitNode split = {};
            split.nodeType = SplitType::SplitType_Categorical;
            return split;
        }

        static SplitNode numerical()
        {
            SplitNode split = {};
            split.nodeType = SplitType::SplitType_Numerical;
            return split;
        }
    };

    template<typename T>
    struct AttributeSplit
    {
        size_t attributeIndex = 0;
        std::vector<SplitNode<T>> nodes;
        double giniIndex = 1.0;

        AttributeSplit() = default;

        explicit AttributeSplit(size_t attrib) : attributeIndex(attrib)
        {

        }
    };


    template<typename T>
    class TreeBuilder
    {
    private:
        bool m_categorical = true;
        size_t m_numericalSplitCount = 2;
        Dataset<T> m_dataset;
        size_t m_attributeCount;

        size_t m_maxHeight;
        size_t m_currentHeight = 0;
        size_t m_minNodeSize;

        SplitNode<T> m_root;

        std::pair<T,T> get_attribute_interval_for_transactions(const Enumerable<size_t> &transactions, const size_t attributeIndex)
        {
            T min = std::numeric_limits<T>::max();
            T max = std::numeric_limits<T>::min();

            for (const size_t &tId : transactions)
            {
                const T &value = m_dataset.transactions[tId].attributeValues[attributeIndex];
                if (value < min)
                    min = value;
                if (value > max)
                    max = value;
            }
            return std::make_pair(min,max);
        }

        SplitNode<T> get_new_split_node()
        {
            SplitNode<T> node = m_categorical ? SplitNode<T>::categorical() : SplitNode<T>::numerical();
            return node;
        }

        std::set<T> get_attribute_values(const Enumerable<size_t> &transactions, const size_t attributeIndex)
        {
            std::set<T> values;
            for (const size_t &tIndex : transactions)
            {
                values.insert(m_dataset.transactions[tIndex].attributeValues[attributeIndex]);
            }
            return values;
        }

        std::set<int> get_found_classes(const Enumerable<size_t> &transactionIds)
        {
            std::set<int> classes;
            for (const size_t &tIndex : transactionIds)
            {
                classes.insert(m_dataset.transactions[tIndex].classIndex);
            }
            return classes;
        }


        double calculate_gini_index(AttributeSplit<T> &attributeSplit, const size_t totalTransactionCount)
        {
            double gini = 0.0;
            for (SplitNode<T> &attributeValueNode : attributeSplit.nodes)
            {
                const auto nodeTransactionCount = attributeValueNode.transactionIds.count();
                double relFreq = static_cast<double>(nodeTransactionCount) /
                                 static_cast<double>(totalTransactionCount);

                auto classesInNode = get_found_classes(attributeValueNode.transactionIds);
                std::map<int, size_t> classFreq = get_class_freq(attributeValueNode);

                double nodeGiniPart = 0.0;
                for (const auto &classFreqPair : classFreq)
                {
                    nodeGiniPart += pow((static_cast<double>(classFreqPair.second) / static_cast<double>(nodeTransactionCount)), 2);
                }
                nodeGiniPart = 1.0 - nodeGiniPart;

                gini += (relFreq * nodeGiniPart);
            }

            return gini;
        }


        std::map<int, size_t> get_class_freq(const SplitNode<T> &childrenNode)
        {
            std::map<int, size_t> classFreq;

            for (const size_t &tId : childrenNode.transactionIds)
            {
                const int classIndex = m_dataset.transactions[tId].classIndex;
                if (classFreq.find(classIndex) == classFreq.end())
                {
                    classFreq[classIndex] = 1;
                }
                else
                {
                    classFreq[classIndex] += 1;
                }
            }
            return classFreq;
        }

        int get_prevailing_class(const SplitNode<T> &childrenNode)
        {
            auto classFreq = get_class_freq(childrenNode);
            size_t maxFreq = 0;
            int maxClass = -1;
            for (const auto &pair:classFreq)
            {
                if (pair.second >= maxFreq)
                {
                    maxFreq = pair.second;
                    maxClass = pair.first;
                }
            }
            always_assert(maxClass != -1);
            return maxClass;
        }

        AttributeSplit<T> categorical_split(SplitNode<T> &node, const size_t attributeIndex)
        {
            always_assert(m_categorical);
            const auto nodeTransactionCount = node.transactionIds.count();
            auto attributeValues = get_attribute_values(node.transactionIds, attributeIndex);
            AttributeSplit<T> attributeSplit(attributeIndex);
            attributeSplit.nodes.resize(attributeValues.size());
            size_t i = 0;
            for (const T &attributeValue : attributeValues)
            {
                SplitNode<T> attributeChildNode = get_new_split_node();
                attributeChildNode.height = node.height + 1;
                if (attributeChildNode.height > m_currentHeight)
                    m_currentHeight = attributeChildNode.height;
                attributeChildNode.attributeValue = attributeValue;
                attributeChildNode.transactionIds = node.transactionIds
                        .where([this, attributeIndex, attributeValue](const size_t &tId)
                               {
                                   return (m_dataset.transactions[tId].attributeValues[attributeIndex] == attributeValue);
                               });

                attributeSplit.nodes[i++] = attributeChildNode;
            }
            attributeSplit.giniIndex = calculate_gini_index(attributeSplit, nodeTransactionCount);
            return attributeSplit;
        }

        T get_interval_size(const T &min, const T &max)
        {
            always_assert(max > min);
            if (min >= 0)
                return (max - min);
            else
            {
                return (max + abs(min));
            }
        }

        AttributeSplit<T> numerical_split(SplitNode<T> &node, const size_t attributeIndex)
        {
            always_assert(!m_categorical);
            const auto nodeTransactionCount = node.transactionIds.count();
            AttributeSplit<T> attributeSplit(attributeIndex);

            const std::pair<T, T> &minMax = get_attribute_interval_for_transactions(node.transactionIds, attributeIndex);
            const T intervalSize = get_interval_size(minMax.first, minMax.second);
            const T stepSize = intervalSize / static_cast<T>(m_numericalSplitCount);
            attributeSplit.nodes.resize(m_numericalSplitCount);

            for (size_t i = 0; i < m_numericalSplitCount; ++i)
            {
                const T from = minMax.first + (i * stepSize);
                const T to = (i == m_numericalSplitCount - 1) ? minMax.second : minMax.first + stepSize + (i * stepSize);

                SplitNode<T> attributeChildNode = get_new_split_node();
                attributeChildNode.height = node.height + 1;
                if (attributeChildNode.height > m_currentHeight)
                    m_currentHeight = attributeChildNode.height;

                attributeChildNode.attributeValue = to;
                attributeChildNode.transactionIds = node.transactionIds
                        .where([this, attributeIndex, from, to](const size_t &tId)
                               {
                                   const T attribValue = m_dataset.transactions[tId].attributeValues[attributeIndex];
                                   return (attribValue > from && attribValue <= to);
                               });

                if (attributeChildNode.transactionIds.count() <= 0)
                {
                    azgra::print_if(true, "No transaction in node.\n");
                }

                attributeSplit.nodes[i] = attributeChildNode;
            }

            attributeSplit.giniIndex = calculate_gini_index(attributeSplit, nodeTransactionCount);
            return attributeSplit;
        }


        void create_best_split(SplitNode<T> &node,
                               std::function<AttributeSplit<T>(SplitNode<T> &, const size_t)> splitFunc)
        {
            const auto nodeTransactionCount = node.transactionIds.count();
            auto classesInNode = get_found_classes(node.transactionIds);
            const auto nodeClassCount = classesInNode.size();

            always_assert(nodeClassCount >= 1);
            if (nodeTransactionCount <= 0 || nodeClassCount == 1)
            {
                // The node is pure.
                node.predicatedClass = *classesInNode.begin();
                return;
            }


            std::vector<AttributeSplit<T>> possibleSplits(m_attributeCount);
            for (size_t attributeIndex = 0; attributeIndex < m_attributeCount; ++attributeIndex)
            {
                const AttributeSplit<T> split = splitFunc(node, attributeIndex);
                possibleSplits[attributeIndex] = split;
            }


            double minGiniIndex = 999.0;
            AttributeSplit<T> bestSplit;
            for (const AttributeSplit<T> &split : possibleSplits)
            {
                if (split.giniIndex < minGiniIndex)
                {
                    minGiniIndex = split.giniIndex;
                    bestSplit = split;
                }
            }
            always_assert(bestSplit.attributeIndex < m_attributeCount);
            node.set_attribute_index(bestSplit.attributeIndex);
            node.children = bestSplit.nodes;

            for (SplitNode<T> &childrenNode : node.children)
            {
                if ((childrenNode.transactionIds.count() < m_minNodeSize) || childrenNode.height > m_maxHeight)
                {
                    childrenNode.predicatedClass = get_prevailing_class(childrenNode);
                    continue;
                }
                create_best_split(childrenNode, splitFunc);
            }
        }

        void build_tree(SplitNode<T> &node)
        {
            switch (node.nodeType)
            {
                case SplitType::SplitType_Categorical:
                {
                    std::function<AttributeSplit<T>(SplitNode<T> &, const size_t)> f = [=](SplitNode<T> &node, const size_t attrib)
                    {
                        return categorical_split(node, attrib);
                    };
                    create_best_split(node, f);
                }
                    break;
                case SplitType::SplitType_Numerical:
                {
                    std::function<AttributeSplit<T>(SplitNode<T> &, const size_t)> f = [=](SplitNode<T> &node, const size_t attrib)
                    {
                        return numerical_split(node, attrib);
                    };
                    create_best_split(node, f);
                }
                    break;
            }
        }

        int internal_classify(const Transaction<T> &transaction, std::function<bool(const T &, const T &)> testFunc)
        {
            SplitNode<T> currentNode = m_root;
            bool found = true;
            while (found)
            {
                found = false;
                if (currentNode.children.size() <= 0)
                    break;
                T value = transaction.attributeValues[currentNode.attributeIndex];
                for (const SplitNode<T> &child : currentNode.children)
                {
                    if (testFunc(child.attributeValue, value))
                    {
                        found = true;
                        currentNode = child;
                        break;
                    }
                }
            }
            return currentNode.predicatedClass;
        }

        int classify_categorical(const Transaction<T> &transaction)
        {
            std::function<bool(const T &, const T &)> test = [=](const T &nodeVal, const T &transVal)
            {
                return (nodeVal == transVal);
            };
            return internal_classify(transaction, test);
        }

        int classify_numerical(const Transaction<T> &transaction)
        {
            std::function<bool(const T &, const T &)> test = [=](const T &nodeVal, const T &transVal)
            {
                return (transVal <= nodeVal);
            };
            return internal_classify(transaction, test);
        }

    public:
        explicit TreeBuilder(const Dataset<T> &dataset, const size_t splitCount = 2)
        {
            always_assert(dataset.transactions.size() > 0);
            m_categorical = !std::is_same<T, float>::value;
            m_dataset = dataset;
            m_numericalSplitCount = splitCount;
            m_attributeCount = m_dataset.transactions[0].attributeValues.size();
        }


        void build(const size_t minimalNodeSize, const size_t maxHeigh)
        {
            m_maxHeight = maxHeigh;
            m_minNodeSize = minimalNodeSize;

            m_root = get_new_split_node();
            m_root.height = 1;
            m_currentHeight = m_root.height;
            m_root.transactionIds = azgra::collection::Enumerable<size_t>::range(0, m_dataset.transactions.size());
            //create_best_split(m_root);
            build_tree(m_root);
            fprintf(stdout, "Built classification tree of height %lu\n", m_currentHeight);
        }


        int classify(const Transaction<T> &transaction)
        {
            return (m_categorical ? classify_categorical(transaction) : classify_numerical(transaction));
        }

        double test_classification(Dataset<T> &testDataset)
        {
            size_t correct = 0;
            for (Transaction<T> &t : testDataset.transactions)
            {
                t.guessedClassIndex = classify(t);
                if (t.guessedClassIndex == t.classIndex)
                    ++correct;
            }
            double precision = static_cast<double> (correct) / static_cast<double   >(testDataset.transactions.size());
            return precision;
        }

    };
}
