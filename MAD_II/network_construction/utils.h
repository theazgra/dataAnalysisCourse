#pragma once

#include "custom_types.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>

template <typename T>
void print_vector(const std::vector<T> &vector)
{
    size_t vs = vector.size();
    for (size_t i = 0; i < vs; i++)
    {
        if (i == vs - 1)
            std::cout << vector[i] << std::endl;
        else
            std::cout << vector[i] << ", ";
    }
}

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
void push_range(std::vector<T> &dst, const std::vector<T> &src)
{
    for (size_t i = 0; i < src.size(); i++)
    {
        dst.push_back(src[i]);
    }
}

template <typename T>
T min(const std::vector<T> &vector)
{
    T min = std::numeric_limits<T>::max();
    for (size_t i = 0; i < vector.size(); i++)
    {
        if (vector[i] < min)
            min = vector[i];
    }
    return min;
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

template <typename T>
std::vector<T> first_of(const std::vector<T> &src, const bool decreasing = false, const uint count = 5)
{
    std::vector<T> copy = std::vector<T>(src);
    std::vector<T> result;
    result.reserve(count);
    std::sort(copy.begin(), copy.end());

    size_t size = copy.size();
    size_t end = size - 1;
    if (decreasing)
    {
        for (size_t i = 0; i < count; i++)
        {
            if (i > end)
                break;
            result.push_back(copy[(end - i)]);
        }
    }
    else
    {
        for (size_t i = 0; i < count; i++)
        {
            if (i >= size)
                break;
            result.push_back(copy[i]);
        }
    }
    return result;
}
