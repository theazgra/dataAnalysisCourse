#pragma once

#include "custom_types.h"
#include <vector>

template <typename T>
bool find(const std::vector<T> &vector, const T &element)
{
    for (size_t i = 0; i < vector.size(); i++)
    {
        if (vector[i] == element)
            return true;
    }
    return false;
}

template <typename T>
T sum(const std::vector<T> &vector)
{
    T result = 0;
    for (size_t i = 0; i < vector.size(); i++)
    {
        result += vector[i];
    }
    return result;
}

template <typename T>
uint count(const std::vector<T> &vector, const T element)
{
    uint result = 0;
    for (size_t i = 0; i < vector.size(); i++)
    {
        if (vector[i] == element)
        {
            result++;
        }
    }
    return result;
}

template <typename TKey, typename TValue>
bool contains_key(const std::map<TKey, TValue> &dict, TKey key)
{
    return !(dict.find(key) == dict.end());
}
