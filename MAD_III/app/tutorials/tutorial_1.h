#pragma once

#include "../data_analysis/patterns.h"

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

void tutorial_1()
{
    // 1. Vygenerujte všechny kombinace bez opakování o délce 3 z 6 možných.
    auto generatedSubsets = azgra::collection::generate_subsets_of_size<int>({1, 2, 3, 4, 5, 6}, 3);
    print_subsets(generatedSubsets);
    const char *transactionFile = "/mnt/d/codes/git/dataAnalysisCourse/MAD_III/data/chess.dat";
    float minSupp = 0.950f;
    float minConf = 0.500f;

    PatternMiner<int> patternMiner;
    patternMiner.set_line_parse_function([](const azgra::string::SmartStringView<char> &line)
                                         {
                                             auto lineCells = line.split(" ");
                                             std::set<int> items;
                                             for (auto &lineCell : lineCells)
                                             {
                                                 if (lineCell.equals(""))
                                                 {
                                                     continue;
                                                 }
                                                 items.insert(atoi(lineCell.data()));
                                             }
                                             return items;
                                         });

    size_t loadedTransactionsCount = patternMiner.load_transactions(transactionFile);
    fprintf(stdout, "Loaded %lu transactions\n", loadedTransactionsCount);

    // 2. Na jednom z testovacích souborů (chess, connect) vygenerujte četné vzory a vypočtěte Support.
    patternMiner.generate_frequent_item_sets(minSupp);
    auto FIS = patternMiner.get_fis();

    for (size_t i = 0; i < FIS.size(); ++i)
    {
        const std::vector<ItemSubset <int>> level = FIS[i];
        if (!level.empty())
        {
            fprintf(stdout, "Subsets of size: %lu\n", i);
            for (const ItemSubset<int> &subset : level)
            {
                subset.print();
            }
        }
    }
    // 3. Z vygenerovaných četných vzorů vypište pravidla a jejich Confidence
    patternMiner.generate_association_rules(minConf);
    auto assocRulesLevels = patternMiner.get_association_rules();
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