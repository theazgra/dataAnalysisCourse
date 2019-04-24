#include "simple_string.h"

namespace azgra
{
//TODO: Add asserts.

char *SimpleString::alloc_string(const size_t &length)
{
    assert(length > 0);
    char *memory = static_cast<char *>(::operator new(sizeof(char) * length));
    return memory;
}

char *SimpleString::realloc_string(const size_t &length, const char *oldString, const size_t copyLen)
{
    assert(length > copyLen);
    char *memory = alloc_string(length);
    for (size_t i = 0; i < copyLen; i++)
        memory[i] = oldString[i];
    return memory;
}

void SimpleString::free_string(char *memory)
{
    if (memory != nullptr)
    {
        ::operator delete(memory);
        memory = nullptr;
    }
}

size_t SimpleString::c_string_length(const char *cString)
{
    size_t len = 0;
    int i = 0;

    for (i = 0; cString[i] != 0; i++)
        len++;

    return len;
}

void SimpleString::internal_initalize(const char *string)
{
    size_t inputStringLen = c_string_length(string);

    _length = inputStringLen;
    _string = alloc_string(_length + 1);

    for (size_t i = 0; i < inputStringLen; i++)
        _string[i] = string[i];

    _string[_length] = '\000';
}

SimpleString::SimpleString(char *cString, const size_t length, bool noAlloc)
{
    assert(noAlloc);
    _string = cString;
    _length = length;
}

SimpleString::operator const char *() const
{
    return _string;
}

char &SimpleString::operator[](const int &index)
{
    return _string[index];
}

void SimpleString::operator=(const char *cString)
{
    free_string(_string);
    internal_initalize(cString);
}

void SimpleString::operator+=(const char *cString)
{
    concat(cString);
}

SimpleString SimpleString::operator+(const char *cString) const
{
    SimpleString result(_string);
    result.concat(cString);
    return result;
}

bool SimpleString::operator==(const char *cString) const
{
    return equals(cString);
}
bool SimpleString::operator==(const SimpleString &string) const
{
    return equals(string);
}

SimpleString::SimpleString(const char *cString)
{
    internal_initalize(cString);
}

SimpleString::~SimpleString()
{
    printf("Free called\n");
    free_string(_string);
    _length = 0;
}

void SimpleString::internal_concat(const char *string, const size_t length)
{
    if (length == 0)
        return;

    size_t newLength = _length + length + 1;
    char *newString = realloc_string(newLength, _string, _length);

    free_string(_string);

    _string = newString;

    for (size_t i = _length; i < newLength - 1; i++)
        _string[i] = string[i - _length];

    _string[newLength - 1] = '\000';

    _length = newLength - 1;
}

void SimpleString::concat(const char *cString)
{
    size_t cStringLen = c_string_length(cString);
    internal_concat(cString, cStringLen);
}

const char *SimpleString::get_c_string() const
{
    return _string;
}

size_t SimpleString::length() const
{
    return _length;
}

int SimpleString::index_of(const char &c, const size_t fromIndex) const
{
    if (fromIndex >= _length)
        return -1;

    for (size_t i = fromIndex; i < _length; i++)
    {
        if (_string[i] == c)
        {
            return i;
        }
    }
    return -1;
}

int SimpleString::index_of(const char *string, const size_t fromIndex) const
{
    size_t matchLen = c_string_length(string);
    if (matchLen > _length || fromIndex >= _length)
    {
        return -1;
    }

    size_t searchFrom = fromIndex;
    int indexOfFirstChar = index_of(string[0], searchFrom);
    bool found = false;

    while (indexOfFirstChar != -1)
    {
        if (indexOfFirstChar + (matchLen - 1) > _length)
            return -1;

        found = true;
        for (size_t matchIndex = 1; matchIndex < matchLen; matchIndex++)
        {
            if (_string[indexOfFirstChar + matchIndex] != string[matchIndex])
            {
                found = false;
                break;
            }
        }

        if (found)
            return indexOfFirstChar;
        else
        {
            searchFrom = indexOfFirstChar + 1;
            indexOfFirstChar = index_of(string[0], searchFrom);
        }
    }

    return -1;
}

int SimpleString::last_index_of(const char &c, const size_t fromIndex) const
{
    return last_index_of(c, fromIndex, _length - 1);
}

int SimpleString::last_index_of(const char &c, const size_t fromIndex, const size_t toIndex) const
{
    if (fromIndex >= _length)
        return -1;

    for (int i = (int)toIndex; i >= (int)fromIndex; i--)
    {
        if (_string[i] == c)
        {
            return i;
        }
    }
    return -1;
}

int SimpleString::last_index_of(const char *string, const size_t fromIndex) const
{
    size_t matchLen = c_string_length(string);
    if (matchLen > _length || fromIndex >= _length)
    {
        return -1;
    }

    size_t searchUpperBound = _length - 1;
    int indexOfLastChar = last_index_of(string[matchLen - 1], 0, searchUpperBound);
    bool found = false;

    while (indexOfLastChar != -1)
    {
        if (indexOfLastChar - (matchLen - 1) < fromIndex)
            return -1;

        found = true;
        for (size_t matchIndex = 1; matchIndex < matchLen; matchIndex++)
        {
            if (_string[indexOfLastChar - matchIndex] != string[matchLen - matchIndex - 1])
            {
                found = false;
                break;
            }
        }

        if (found)
            return indexOfLastChar - (matchLen - 1);
        else
        {
            searchUpperBound = indexOfLastChar - 1;
            indexOfLastChar = last_index_of(string[matchLen - 1], 0, searchUpperBound);
        }
    }

    return -1;
}

bool SimpleString::contains(const char &c) const
{
    return (index_of(c) != -1);
}

bool SimpleString::contains(const char *string) const
{
    return (index_of(string) != -1);
}

size_t SimpleString::count(const char &c) const
{
    size_t result = 0;
    size_t fromIndex = 0;

    int matchIndex = index_of(c, fromIndex);

    while (matchIndex != -1)
    {
        ++result;
        fromIndex = matchIndex + 1;
        matchIndex = index_of(c, fromIndex);
    }
    return result;
}

size_t SimpleString::count(const char *string) const
{
    size_t result = 0;
    size_t fromIndex = 0;
    size_t matchLen = c_string_length(string);
    if (matchLen > _length)
        return 0;

    int matchIndex = index_of(string, fromIndex);
    while (matchIndex != -1)
    {
        ++result;
        fromIndex = matchIndex + matchLen;
        matchIndex = index_of(string, fromIndex);
    }
    return result;
}

bool SimpleString::starts_with(const char &c) const
{
    return (index_of(c) == 0);
}

bool SimpleString::starts_with(const char *string) const
{
    return (index_of(string) == 0);
}

bool SimpleString::ends_with(const char &c) const
{
    return (last_index_of(c) == (long)(_length - 1));
}

bool SimpleString::ends_with(const char *string) const
{
    return (last_index_of(string) == (long)(_length - c_string_length(string)));
}

bool SimpleString::equals(const SimpleString &string) const
{
    return equals(string._string);
}

bool SimpleString::equals(const char *string) const
{
    if (_length != c_string_length(string))
        return false;

    for (size_t i = 0; i < _length; i++)
    {
        if (_string[i] != string[i])
            return false;
    }
    return true;
}

void SimpleString::to_upper()
{
    for (size_t i = 0; i < _length; i++)
    {
        char c = _string[i];
        if (c >= 'a' && c <= 'z')
            _string[i] -= AsciiCaseOffset;
    }
}

void SimpleString::to_lower()
{
    for (size_t i = 0; i < _length; i++)
    {
        char c = _string[i];
        if (c >= 'A' && c <= 'Z')
            _string[i] += AsciiCaseOffset;
    }
}

void SimpleString::replace(const char &oldChar, const char &newChar)
{
    for (size_t i = 0; i < _length; i++)
    {
        if (_string[i] == oldChar)
            _string[i] = newChar;
    }
}

// void SimpleString::replace(const SimpleString &oldString, const SimpleString &newString)
// {
//     replace(oldString._string, newString._string);
// }

void SimpleString::replace(const char *oldString, const char *newString)
{
    size_t matchLen = c_string_length(oldString);
    size_t matchCount = count(oldString);
    if (matchCount == 0 || matchLen == 0)
        return;

    size_t replaceStringLen = c_string_length(newString);
    size_t newLength = _length - (matchCount * matchLen) + (matchCount * replaceStringLen);
    char *newStringMemory = alloc_string(newLength);

    size_t searchFrom = 0;
    size_t newStringIndex = 0;
    int index = index_of(oldString, searchFrom);

    while (index != -1)
    {
        // Copy all in front of match.
        for (int i = searchFrom; i < index; i++)
            newStringMemory[newStringIndex++] = _string[i];

        // Copy replace string into new string.
        for (size_t i = 0; i < replaceStringLen; i++)
        {
            newStringMemory[newStringIndex++] = newString[i];
        }

        searchFrom = index + matchLen;
        index = index_of(oldString, searchFrom);
    }

    // Copy remaining old content.
    for (size_t i = searchFrom; i < _length; i++)
        newStringMemory[newStringIndex++] = _string[i];

    free_string(_string);
    _string = newStringMemory;
    _length = newLength;
}

void SimpleString::remove(const char &c)
{
    size_t removeCount = count(c);
    if (removeCount == 0)
        return;

    size_t newLength = _length - removeCount;
    char *newString = alloc_string(newLength);
    size_t index = 0;
    for (size_t i = 0; i < _length; i++)
    {
        if (_string[i] != c)
        {
            newString[index++] = _string[i];
        }
    }

    free_string(_string);
    _length = newLength;
    _string = newString;
}

void SimpleString::remove(const char *string)
{
    size_t matchLen = c_string_length(string);
    if (matchLen == 0)
        return;

    size_t matchCount = count(string);
    size_t newLen = _length - (matchCount * matchLen);

    char *newString = alloc_string(newLen);

    size_t searchFrom = 0;
    size_t newStringIndex = 0;
    int index = index_of(string, searchFrom);

    while (index != -1)
    {
        // Copy all in front of match.
        for (int i = searchFrom; i < index; i++)
            newString[newStringIndex++] = _string[i];

        searchFrom = index + matchLen;
        index = index_of(string, searchFrom);
    }

    // Copy remaining old content.
    for (size_t i = searchFrom; i < _length; i++)
        newString[newStringIndex++] = _string[i];

    free_string(_string);
    _string = newString;
    _length = newLen;
}
SimpleString SimpleString::substring(const size_t fromIndex) const
{
    assert(fromIndex < _length);
    size_t len = _length - fromIndex;

    char *subsMemory = alloc_string(len);
    SimpleString result(subsMemory);
    for (size_t i = fromIndex; i < _length; i++)
    {
        result[i - fromIndex] = _string[i];
    }
    return result;
}

SimpleString SimpleString::substring(const size_t fromIndex, const size_t length) const
{
    assert(fromIndex < _length && (fromIndex + length) <= _length);

    char *subsMemory = alloc_string(length);
    SimpleString result(subsMemory);

    for (size_t i = fromIndex; i < fromIndex + length; i++)
    {
        result[i - fromIndex] = _string[i];
    }

    return result;
}

SimpleString SimpleString::replicate(const char *cString, const int replicationCount)
{
    size_t replLen = c_string_length(cString);
    size_t resultLen = replLen * replicationCount;
    assert(replLen > 0 && replicationCount > 0);
    char *resultMemory = alloc_string(resultLen);

    SimpleString result(resultMemory, resultLen, true);

    for (int repl = 0; repl < replicationCount; repl++)
    {
        for (size_t charIndex = 0; charIndex < replLen; charIndex++)
        {
            result[(repl * replLen) + charIndex] = cString[charIndex];
        }
    }
    return result;
}

std::vector<std::shared_ptr<SimpleString>> SimpleString::split(const char &separator) const
{
    std::vector<std::shared_ptr<SimpleString>> result;
    if (count(separator) <= 0)
        return result;

    size_t from = 0;
    size_t match = index_of(separator, from);

    while (match != -1)
    {
        std::shared_ptr<SimpleString> s(new SimpleString(substring(from, match - from)));

        result.push_back(s);
        from = match + 1;
        match = index_of(separator, from);
    }

    if (_length - from > 0)
    {
        result.push_back(std::shared_ptr<SimpleString>(new SimpleString(substring(from))));
    }

    return result;
}

/*
static SimpleString SimpleString::empty();
*/
} // namespace azgra
