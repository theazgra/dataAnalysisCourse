#pragma once

#include <cstdio>
#include <utility>
#include <azgra/collection/set_utilities.h>
#include <azgra/io/text_file_functions.h>
#include <azgra/collection/vector_utilities.h>
#include <azgra/collection/vector_linq.h>
#include <sstream>

template<typename T>
using Transaction = std::set<T>;

#define MAKE_ASSOCIATION_RULE(x, y) std::make_pair<std::vector<int>, std::vector<int>>(std::vector<int>(x), std::vector<int>(y))

template<typename ItemType>
struct AssociationRule
{
    std::vector<ItemType> lhs;
    std::vector<ItemType> rhs;
    float confidence = 0.0f;

    AssociationRule()
    {}

    AssociationRule(std::vector<ItemType> _lhs, std::vector<ItemType> _rhs, float conf) : lhs(_lhs), rhs(_rhs), confidence(conf)
    {}
};

template<typename ItemType>
using AssociationRules = std::vector<AssociationRule<ItemType>>;

template<typename ItemType>
struct Subset
{
    std::set<ItemType> items;
    float support;

    Subset(std::set<ItemType> _items, float _support) : items(std::move(_items)), support(_support)
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

#define PRINT_SIZE_T(x) fprintf(stdout, "%lu\n", x);

template<typename T = int>
static void print_subsets(const std::vector<std::vector<T>> &subsets, const char *format = "%i, ")
{
    for (const std::vector<T> &subset : subsets)
    {
        for (const T &subsetItem : subset)
        {
            fprintf(stdout, format, subsetItem);
        }
        fprintf(stdout, "\n");
    }
}

template<typename T = int>
static void print_subsets(const std::vector<std::set<T>> &subsets, const char *format = "%i, ")
{
    for (const std::set<T> &subset : subsets)
    {
        for (const T &subsetItem : subset)
        {
            fprintf(stdout, format, subsetItem);
        }
        fprintf(stdout, "\n");
    }
}

std::set<char> get_items(const std::vector<Transaction<int>> &transactions)
{
    std::set<char> itemSet;
    for (const Transaction<int> &t : transactions)
    {
        for (const char item : t)
        {
            itemSet.insert(item);
        }
    }
    return itemSet;
}

float subset_support(const std::vector<Transaction<int>> &transactions, const Transaction<int> &subset)
{
    using namespace azgra::collection;
    size_t transactionCount = transactions.size();
    size_t occurenceCount = 0;
    for (const Transaction<int> &trans : transactions)
    {
        std::function<bool(const int)> fn = [trans](const int c)
        {
            return (trans.find(c) != trans.end());
        };

        if (all(set_as_vector(subset), fn))
        {
            ++occurenceCount;
        }
    }
    float support = (float) occurenceCount / (float) transactionCount;
    return support;
}

std::vector<Transaction<int>> load_transactions(const char *dataFile)
{
    std::function<Transaction<int>(const azgra::string::SmartStringView<char> &)>
            lineToTransaction = [](const azgra::string::SmartStringView<char> &line)
    {
        auto lineCells = line.split(" ");
        std::set<int> items;
        for (size_t i = 0; i < lineCells.size(); ++i)
        {
            if (lineCells[i].equals(""))
            {
                continue;
            }
            items.insert(atoi(lineCells[i].data()));
        }
        return items;
    };

    std::vector<Transaction<int>> transactions = azgra::io::parse_by_lines(dataFile, lineToTransaction);
    return transactions;
}

std::vector<std::vector<Subset<int>>> generate_frequent_item_sets(const std::vector<Transaction<int>> &transactions, float minSupport)
{
    bool verbose = false;
    auto items = get_items(transactions);
    size_t itemCount = items.size();
    fprintf(stdout, "Item count: %lu\n", itemCount);

    std::vector<std::vector<Subset<int>>> subsets;
    subsets.emplace_back();

    for (size_t treeLevel = 0; treeLevel < itemCount; ++treeLevel)
    {
        std::vector<Subset<int>> levelSubsets;
        if (treeLevel == 0)
        {
            for (const char item : items)
            {
                float itemSupport = subset_support(transactions, {item});
                if (itemSupport >= minSupport)
                {
                    Subset<int> subset({item}, itemSupport);
                    levelSubsets.push_back(subset);
                }
            }
            if (levelSubsets.size() == 0)
            {
                fprintf(stdout, "No items satisfying minimum support.\n");
                return subsets;
            }
        }
        else
        {
            size_t prevLevelSubsetCount = subsets[treeLevel].size();
            for (size_t currentSubsetIndex = 0;
                 currentSubsetIndex < prevLevelSubsetCount;
                 ++currentSubsetIndex)
            {
                const Subset<int> current = subsets[treeLevel][currentSubsetIndex];

//                if (verbose)
//                {
//                    fprintf(stdout, "current subset: ");
//                    current.print();
//                }


                for (size_t neighborSubsetIndex = currentSubsetIndex + 1;
                     neighborSubsetIndex < (currentSubsetIndex + 1) + prevLevelSubsetCount;
                     ++neighborSubsetIndex)
                {

                    if (currentSubsetIndex == neighborSubsetIndex)
                        continue;
                    const Subset<int> neighbor = subsets[treeLevel][neighborSubsetIndex % prevLevelSubsetCount];

                    std::set<int> possiblyNewSubset;
                    {
                        std::vector<int> tmp;
                        std::set_union(current.items.begin(), current.items.end(),
                                       neighbor.items.begin(), neighbor.items.end(),
                                       std::back_inserter(tmp));
                        possiblyNewSubset = azgra::collection::vector_as_set(tmp);
                    }
                    if (possiblyNewSubset.size() != treeLevel + 1)
                        continue;


                    bool isNew = true;
                    for (const Subset<int> &levelSubset : levelSubsets)
                    {
                        if (levelSubset.items == possiblyNewSubset)
                        {
                            isNew = false;
                            break;
                        }
                    }
                    if (isNew)
                    {
                        float support = subset_support(transactions, possiblyNewSubset);
                        if (support >= minSupport)
                        {

                            levelSubsets.push_back( // NOLINT(hicpp-use-emplace,modernize-use-emplace)
                                    Subset<int>(possiblyNewSubset, support));

//                            if (verbose)
//                            {
//                                fprintf(stdout, "\t successful merge  with:  ");
//                                neighbor.print();
//                            }
                        }
//                        else
//                        {
//                            if (verbose)
//                            {
//                                fprintf(stdout, "\t failed, low support, with:  ");
//                                neighbor.print();
//                            }
//                        }
                    }
//                    else
//                    {
//                        if (verbose)
//                        {
//                            fprintf(stdout, "\t failed, already exists, with:  ");
//                            neighbor.print();
//                        }
//                    }
                }
            }
        }

        subsets.push_back(levelSubsets);
    }
    return subsets;
}

std::vector<AssociationRules<int>> generate_association_rules(const std::vector<Transaction<int>> &transactions,
                                                              const std::vector<std::vector<Subset<int>>> &fis,
                                                              const float minConf)
{
    std::vector<AssociationRules<int>> treeLevelsAssocRules;

    for (const std::vector<Subset<int>> &treeLevel : fis)
    {
        // We dont care about empty tree levels and item sets with just one item.
        if (treeLevel.empty() || treeLevel[0].items.size() == 1)
            continue;


        size_t itemSetSize = treeLevel[0].items.size();
        assert(itemSetSize > 1);

        AssociationRules<int> levelRules;

        for (const Subset<int> &itemSet : treeLevel)
        {
            auto itemSetSubsets = azgra::collection::generate_subsets_of_size(azgra::collection::set_as_vector(itemSet.items),
                                                                              (itemSetSize - 1));

            float XuY_Support = itemSet.support;
            for (const std::vector<int> &itemSetSubset: itemSetSubsets)
            {
                std::set<int> itemSetSubsetAsSet = azgra::collection::vector_as_set(itemSetSubset);
                float X_Support = subset_support(transactions, itemSetSubsetAsSet);
                float ruleConf = XuY_Support / X_Support;
                if (ruleConf >= minConf)
                {
                    auto rightSideOfTheRule = azgra::collection::except(itemSet.items.begin(), itemSet.items.end(),
                                                                        itemSetSubsetAsSet.begin(), itemSetSubsetAsSet.end()
                    );
                    levelRules.push_back(AssociationRule(itemSetSubset, rightSideOfTheRule, ruleConf));
                }
            }
        }

        treeLevelsAssocRules.push_back(levelRules);
    }
    return treeLevelsAssocRules;
}

void tutorial_1()
{
    const char *transactionFile = "/mnt/d/gitrepos/dataAnalysisCourse/MAD_III/data/chess.dat";
    float minSupp = 0.950f;
    float minConf = 0.500f;
    // 1. Vygenerujte všechny kombinace bez opakování o délce 3 z 6 možných.
//    auto generatedSubsets = azgra::collection::generate_subsets_of_size<int>({1, 2, 3, 4, 5, 6}, 3);
//    print_subsets(generatedSubsets);

    // 2. Na jednom z testovacích souborů (chess, connect) vygenerujte četné vzory a vypočtěte Support.
    auto transactions = load_transactions(transactionFile);
    auto FIS = generate_frequent_item_sets(transactions, minSupp);

    for (size_t i = 0; i < FIS.size(); ++i)
    {
        const std::vector<Subset<int>> level = FIS[i];
        if (!level.empty())
        {
            fprintf(stdout, "Subsets of size: %lu\n", i);
            for (const Subset<int> &subset : level)
            {
                subset.print();
            }
        }
    }
    // 3. Z vygenerovaných četných vzorů vypište pravidla a jejich Confidence
    auto assocRulesLevels = generate_association_rules(transactions, FIS, minConf);
    for (const auto &assocRuleLevel : assocRulesLevels)
    {
        if (assocRuleLevel.empty())
            continue;
        size_t ruleSize = assocRuleLevel[0].lhs.size() + 1;
        fprintf(stdout, "Printing rules of size: %lu\n", ruleSize);

        for (const auto &assocRule : assocRuleLevel)
        {
            std::stringstream ss;
            ss << "[";
            for (const auto &lhsItem : assocRule.lhs)
                ss << lhsItem << ",";

            ss << "] --> [";
            for (const auto &rhsItem : assocRule.rhs)
                ss << rhsItem << ",";
            ss << "]";
            fprintf(stdout, "%s\t Confidence: %.4f\n", ss.str().c_str(), assocRule.confidence);
        }
        fprintf(stdout, "\n");
    }


};