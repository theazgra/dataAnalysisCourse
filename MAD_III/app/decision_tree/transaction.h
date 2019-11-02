#pragma once

#include <vector>

namespace DecisionTree
{
    template<typename T = char>
    struct Transaction
    {
        std::vector<T> attributeValues;
        int classIndex = -1;
        int guessedClassIndex = -1;

        Transaction() = default;

        explicit Transaction(std::vector<T> _attributeValues, const int _classIndex) :
                attributeValues(std::move(_attributeValues)), classIndex(_classIndex)
        {
        }
    };
}
