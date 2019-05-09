#pragma once
#include <networkLib/libs.h>

#include <iostream>
#include <limits>
namespace azgra::networkLib
{

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
void print_vector(const std::vector<T> &vector, const char *itemFormat)
{
    size_t vs = vector.size();
    for (size_t i = 0; i < vs; i++)
    {
        fprintf(stdout, itemFormat, vector[i]);
    }
    fprintf(stdout, "\n");
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
int find_index(const std::vector<T> &vector, const T &element)
{
    for (size_t i = 0; i < vector.size(); i++)
    {
        if (vector[i] == element)
            return i;
    }
    return -1;
}

template <typename T>
void push_back_if_not_found(std::vector<T> &vector, const T &element)
{
    if (!find(vector, element))
    {
        vector.push_back(element);
    }
}

template <typename T>
std::vector<T> except(const std::vector<T> &copySrc, const std::vector<T> &except)
{
    std::vector<T> result;
    for (size_t i = 0; i < copySrc.size(); i++)
    {
        if (!find(except, copySrc[i]))
        {
            result.push_back(copySrc[i]);
        }
    }
    return result;
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
uint count(const std::vector<T> &vector, const T &element)
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
bool contains_key(const std::map<TKey, TValue> &dict, TKey &key)
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

template <typename T>
inline void max(T &src, const T test) { src = (test > src) ? test : src; }
template <typename T>
inline void min(T &src, const T test) { src = (test < src) ? test : src; }
template <typename T>
inline T min_from(const T &a, const T &b) { return a < b ? a : b; }

}; // namespace azgra::networkLib