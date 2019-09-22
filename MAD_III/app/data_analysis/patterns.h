#pragma once

#include <cstdio>
#include <utility>
#include <azgra/collection/set_utilities.h>
#include <azgra/io/text_file_functions.h>
#include <azgra/collection/vector_utilities.h>
#include <azgra/collection/vector_linq.h>
#include <sstream>

template<typename ItemType>
using Transaction = std::set<ItemType>;

template<typename ItemType>
struct AssociationRule
{
    std::vector<ItemType> lhs;
    std::vector<ItemType> rhs;
    float confidence = 0.0f;

    AssociationRule() = default;

    AssociationRule(std::vector<ItemType> _lhs, std::vector<ItemType> _rhs, float conf) : lhs(std::move(_lhs)), rhs(std::move(_rhs)),
                                                                                          confidence(conf)
    {}
};

template<typename ItemType>
struct ItemSubset
{
    std::set<ItemType> items;
    float support = 0.0f;

    ItemSubset() = default;

    ItemSubset(std::set<ItemType> _items, float _support) : items(std::move(_items)), support(_support)
    {}

    void print() const
    {
        fprintf(stdout, "Subset support: %.4f\t[", support);
        for (const ItemType &item : items)
        {
            fprintf(stdout, "%i,", item);
        }
        fprintf(stdout, "]\n");
    }

};

template<typename ItemType>
class PatternMiner
{
private:
    std::set<ItemType> m_items;
    std::vector<Transaction<ItemType>> m_transactions;
    std::vector<std::vector<ItemSubset<ItemType>>> m_fis;
    std::vector<std::vector<AssociationRule<ItemType>>> m_associationRules;

    size_t m_itemCount = 0;
    std::function<Transaction<ItemType>(const azgra::string::SmartStringView<char> &)> m_lineParseFunction;

    void load_items_from_transactions()
    {
        if (m_transactions.size() == 0)
        {
            return;
        }
        for (const Transaction<int> &t : m_transactions)
        {
            for (const char item : t)
            {

                m_items.insert(item);
            }
        }
        m_itemCount = m_items.size();
    }

    float subset_support(const Transaction<ItemType> &subset)
    {
        using namespace azgra::collection;
        size_t transactionCount = m_transactions.size();
        size_t occurenceCount = 0;
        for (const Transaction<ItemType> &trans : m_transactions)
        {
            std::function<bool(const ItemType )> fn = [trans](const ItemType &item)
            {
                return (trans.find(item) != trans.end());
            };


            if (all(set_as_vector(subset), fn))
            {
                ++occurenceCount;
            }
        }
        float support = (float) occurenceCount / (float) transactionCount;
        return support;
    }

public:
    size_t load_transactions(const char *dataFile)
    {
        m_transactions.clear();
        m_transactions = azgra::io::parse_by_lines(dataFile, m_lineParseFunction);
        load_items_from_transactions();
        return m_transactions.size();
    }

    void generate_frequent_item_sets(float minSupport)
    {
        m_fis.clear();
        m_fis.emplace_back();

        for (size_t treeLevel = 0; treeLevel < m_itemCount; ++treeLevel)
        {
            std::vector<ItemSubset<ItemType>> levelSubsets;
            if (treeLevel == 0)
            {
                for (const ItemType &item : m_items)
                {
                    float itemSupport = subset_support({item});
                    if (itemSupport >= minSupport)
                    {
                        ItemSubset<ItemType> subset({item}, itemSupport);
                        levelSubsets.push_back(subset);
                    }
                }
            }
            else
            {
                size_t prevLevelSubsetCount = m_fis[treeLevel].size();
                for (size_t currentSubsetIndex = 0;
                     currentSubsetIndex < prevLevelSubsetCount;
                     ++currentSubsetIndex)
                {
                    const ItemSubset<ItemType> current = m_fis[treeLevel][currentSubsetIndex];

                    for (size_t neighborSubsetIndex = currentSubsetIndex + 1;
                         neighborSubsetIndex < (currentSubsetIndex + 1) + prevLevelSubsetCount;
                         ++neighborSubsetIndex)
                    {

                        if (currentSubsetIndex == neighborSubsetIndex)
                            continue;
                        const ItemSubset<ItemType> neighbor = m_fis[treeLevel][neighborSubsetIndex % prevLevelSubsetCount];

                        std::set<ItemType> possiblyNewSubset;
                        {
                            std::vector<ItemType> tmp;
                            std::set_union(current.items.begin(), current.items.end(),
                                           neighbor.items.begin(), neighbor.items.end(),
                                           std::back_inserter(tmp));
                            possiblyNewSubset = azgra::collection::vector_as_set(tmp);
                        }

                        if (possiblyNewSubset.size() != treeLevel + 1)
                            continue;

                        bool isNew = true;
                        for (const ItemSubset<ItemType> &levelSubset : levelSubsets)
                        {
                            if (levelSubset.items == possiblyNewSubset)
                            {
                                isNew = false;
                                break;
                            }
                        }
                        if (isNew)
                        {
                            float support = subset_support(possiblyNewSubset);
                            if (support >= minSupport)
                            {
                                levelSubsets.push_back(ItemSubset<ItemType>(possiblyNewSubset, support));
                            }
                        }
                    }
                }
            }
            m_fis.push_back(levelSubsets);
        }
    }

    void generate_association_rules(const float minConf)
    {
        if (m_fis.size() == 0)
        {
            return;
        }
        for (const std::vector<ItemSubset<ItemType>> &treeLevel : m_fis)
        {
            // We dont care about empty tree levels and item sets with just one item.
            if (treeLevel.empty() || treeLevel[0].items.size() == 1)
            {
                continue;
            }
            size_t itemSetSize = treeLevel[0].items.size();
            assert(itemSetSize > 1);

            std::vector<AssociationRule<ItemType>> levelRules;

            for (const ItemSubset<ItemType> &itemSet : treeLevel)
            {
                auto itemSetSubsets = azgra::collection::generate_subsets_of_size(azgra::collection::set_as_vector(itemSet.items),
                                                                                  (itemSetSize - 1));

                float XuY_Support = itemSet.support;
                for (const std::vector<ItemType> &itemSetSubset : itemSetSubsets)
                {
                    std::set<ItemType> itemSetSubsetAsSet = azgra::collection::vector_as_set(itemSetSubset);
                    float X_Support = subset_support(itemSetSubsetAsSet);
                    float ruleConf = XuY_Support / X_Support;
                    if (ruleConf >= minConf)
                    {
                        auto rightSideOfTheRule = azgra::collection::except(itemSet.items.begin(), itemSet.items.end(),
                                                                            itemSetSubsetAsSet.begin(), itemSetSubsetAsSet.end());

                        levelRules.push_back(AssociationRule(itemSetSubset, rightSideOfTheRule, ruleConf)
                        );
                    }
                }
            }

            m_associationRules.push_back(levelRules);
        }
    }


    void set_line_parse_function(std::function<Transaction<ItemType>(const azgra::string::SmartStringView<char> &)> fn)
    {
        m_lineParseFunction = fn;
    }

    const std::vector<std::vector<ItemSubset<ItemType>>> &get_fis()
    {
        return m_fis;
    };

    const std::vector<std::vector<AssociationRule<ItemType>>> &get_association_rules()
    {
        return m_associationRules;
    }
};

//template<typename T = int>
//static void print_subsets(const std::vector<std::vector<T>> &subsets, const char *format = "%i, ")
//{
//    for (const std::vector<T> &subset : subsets)
//    {
//        for (const T &subsetItem : subset)
//        {
//            fprintf(stdout, format, subsetItem);
//        }
//        fprintf(stdout, "\n");
//    }
//}
//
//template<typename T = int>
//static void print_subsets(const std::vector<std::set<T>> &subsets, const char *format = "%i, ")
//{
//    for (const std::set<T> &subset : subsets)
//    {
//        for (const T &subsetItem : subset)
//        {
//            fprintf(stdout, format, subsetItem);
//        }
//        fprintf(stdout, "\n");
//    }
//}






//
//
//void tutorial_1()
//{
//    const char *transactionFile = "/mnt/d/gitrepos/dataAnalysisCourse/MAD_III/data/chess.dat";
//    float minSupp = 0.950f;
//    float minConf = 0.500f;
//    // 1. Vygenerujte všechny kombinace bez opakování o délce 3 z 6 možných.
////    auto generatedSubsets = azgra::collection::generate_subsets_of_size<int>({1, 2, 3, 4, 5, 6}, 3);
////    print_subsets(generatedSubsets);
//
//    // 2. Na jednom z testovacích souborů (chess, connect) vygenerujte četné vzory a vypočtěte Support.
//    auto transactions = load_transactions(transactionFile);
//    auto FIS = generate_frequent_item_sets(transactions, minSupp);
//
//    for (size_t i = 0; i < FIS.size(); ++i)
//    {
//        const std::vector<Subset < int>>
//        level = FIS[i];
//        if (!level.empty())
//        {
//            fprintf(stdout, "Subsets of size: %lu\n", i);
//            for (const Subset<int> &subset : level)
//            {
//                subset.print();
//            }
//        }
//    }
//    // 3. Z vygenerovaných četných vzorů vypište pravidla a jejich Confidence
//    auto assocRulesLevels = generate_association_rules(transactions, FIS, minConf);
//    for (const auto &assocRuleLevel : assocRulesLevels)
//    {
//        if (assocRuleLevel.empty())
//            continue;
//        size_t ruleSize = assocRuleLevel[0].lhs.size() + 1;
//        fprintf(stdout, "Printing rules of size: %lu\n", ruleSize);
//
//        for (const auto &assocRule : assocRuleLevel)
//        {
//            std::stringstream ss;
//            ss << "[";
//            for (const auto &lhsItem : assocRule.lhs)
//                ss << lhsItem << ",";
//
//            ss << "] --> [";
//            for (const auto &rhsItem : assocRule.rhs)
//                ss << rhsItem << ",";
//            ss << "]";
//            fprintf(stdout, "%s\t Confidence: %.4f\n", ss.str().c_str(), assocRule.confidence);
//        }
//        fprintf(stdout, "\n");
//    }
//
//
//};