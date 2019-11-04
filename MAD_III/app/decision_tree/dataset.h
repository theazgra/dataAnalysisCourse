#pragma once

#include "transaction.h"
#include <set>
#include <algorithm>
#include <random>

namespace DecisionTree
{
    template<typename T>
    struct Dataset
    {
        std::vector<Transaction<T>> transactions;
        std::set<int> classes;
        std::set<char> categories;
    };

    /// Return Train,Test datasets from one dataset.
    /// \tparam T Type of transaction item.
    /// \param dataset Original dataset, will be shuffled!
    /// \param testPerc Percentage of test transactions.
    template<typename T>
    std::pair<Dataset<T>, Dataset<T>> split_dataset(Dataset<T> &dataset, const float testPerc = 0.2)
    {
        const auto testDatasetSize = static_cast<size_t>(floor(static_cast<float>(dataset.transactions.size()) * testPerc));
        const auto trainDatasetSize = dataset.transactions.size() - testDatasetSize;

        std::shuffle(dataset.transactions.begin(), dataset.transactions.end(), std::mt19937());

        Dataset<T> test;
        Dataset<T> train;
        test.classes = dataset.classes;
        train.classes = dataset.classes;
        test.transactions.resize(testDatasetSize);
        train.transactions.resize(trainDatasetSize);
        for (size_t i = 0; i < dataset.transactions.size(); ++i)
        {
            if (i < testDatasetSize)
            {
                test.transactions[i] = dataset.transactions[i];
            }
            else
            {
                train.transactions[i - testDatasetSize] = dataset.transactions[i];
            }
        }
        return std::make_pair(train, test);
    }

    template<typename T>
    std::vector<std::pair<Dataset<T>, Dataset<T>>> create_cross_validation_datasets(Dataset<T> &dataset, const size_t k)
    {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(dataset.transactions.begin(), dataset.transactions.end(),g);
        std::vector<std::pair<Dataset<T>, Dataset<T>>> kFold(k);
        const size_t foldSize = dataset.transactions.size() / k;


        for (size_t i = 0; i < k; ++i)
        {
            Dataset<T> test;
            Dataset<T> train;
            test.classes = dataset.classes;
            train.classes = dataset.classes;

            const size_t testFrom = i * foldSize;
            const size_t testTo = foldSize + (i * foldSize);
            for (size_t i = 0; i < dataset.transactions.size(); ++i)
            {
                if (i >= testFrom && i < testTo)
                    test.transactions.push_back(dataset.transactions[i]);
                else
                    train.transactions.push_back(dataset.transactions[i]);
            }
            kFold[i] = std::make_pair(train, test);
        }

        return kFold;
    }
}

