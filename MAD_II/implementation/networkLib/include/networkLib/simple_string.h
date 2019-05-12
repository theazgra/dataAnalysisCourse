#pragma once
#include <stddef.h>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <memory>

namespace azgra
{
constexpr char AsciiCaseOffset = 'a' - 'A';
class SimpleString
{
private:
  /**
   * @brief Internal string memory.
   */
  char *_string = nullptr;
  /**
   * @brief Length of this string. Without null at the end !
   */
  size_t _length = 0;

  /**
   * @brief Allocates memory for string, with size of sizeof(char) * length
   */
  static char *alloc_string(const size_t &length);
  /**
   * @brief Reallocates memory, to bigger size, copying old content.
   */
  static char *realloc_string(const size_t &length, const char *oldString, const size_t copyLen);
  /**
   * @brief Free string memory block.
   */
  static void free_string(char *memory);

  /**
   * @brief Get size of c string.
   * 
   * @param cString String to get size of.
   * @return size_t Size of cString
   */
  static size_t c_string_length(const char *cString);

  /**
   * @brief Internal concatenation function.
   * 
   * @param string String to add to current string.
   * @param length Length of String.
   */
  void internal_concat(const char *string, const size_t length);

  /**
   * @brief Initalize this string memory and fields.
   * 
   * @param string String to initialize from.
   */
  void internal_initalize(const char *string);

  /**
   * @brief Construct a new Simple String with already allocated memory.
   * 
   * @param cString Allocated string.
   * @param length Length of allocated string.
   * @param noAlloc Must be true.
   */
  SimpleString(char *cString, const size_t length, bool noAlloc = false);

public:
  /**
   * @brief Construct a new Simple String object.
   * 
   * @param cString Char array to create string from.
   */
  SimpleString(const char *cString);
  ~SimpleString();

  /**
   * @brief Get the c string object
   * 
   * @return const char* Char array (C like string).
   */
  const char *get_c_string() const;

  /**
   * @brief Concatenate cString to this string.
   * 
   * @param cString String to concatenate.
   */
  void concat(const char *cString);

  /**
   * @brief Get length of this string.
   * 
   * @return size_t Length of string.
   */
  size_t length() const;

  /**
   * @brief Get index of first match.
   * 
   * @param c Char to find.
   * @param fromIndex Index from which search will begin.
   * @return int Index of first match or -1 if not found.
   */
  int index_of(const char &c, const size_t fromIndex = 0) const;

  /**
   * @brief Get index of first matched character in string.
   * 
   * @param string String to find.
   * @param fromIndex Index from which search will begin.
   * @return int Index of first matched character or -1 if not found.
   */
  int index_of(const char *string, const size_t fromIndex = 0) const;

  /**
   * @brief Get index of last match.
   * 
   * @param c Char to find.
   * @param fromIndex Index from which search will begin.
   * @return int Index of last match or -1 if not found.
   */
  int last_index_of(const char &c, const size_t fromIndex = 0) const;

  /**
   * @brief Get index of last match.
   * 
   * @param c Character to find.
   * @param fromIndex Index from which search will begin.
   * @param toIndex Index to which search will continue.
   * @return int Index of last match or -1 if not found.
   */
  int last_index_of(const char &c, const size_t fromIndex, const size_t toIndex) const;

  /**
   * @brief Get index of last match. 
   * 
   * @param string String to find.
   * @param fromIndex Index from which search will begin.
   * @return int Index of last match or -1 if not found.
   */
  int last_index_of(const char *string, const size_t fromIndex = 0) const;

  /**
   * @brief Check if given character is in this string.
   * 
   * @param c Character to find.
   * @return true If char was found in this string.
   * @return false If char wasn't found in this string.
   */
  bool contains(const char &c) const;

  /**
   * @brief Check if given string is in this string.
   * 
   * @param string String to find.
   * @return true If string was found in this string.
   * @return false If string wasn't found in this string.
   */
  bool contains(const char *string) const;

  /**
   * @brief Get number of times character is contained in this string.
   * 
   * @param c Char to find.
   * @return size_t Number of occurencies.
   */
  size_t count(const char &c) const;

  /**
   * @brief Get number of times string is contained in this string.
   * 
   * @param string String to find.
   * @return size_t Number of occurencies.
   */
  size_t count(const char *string) const;

  /**
   * @brief Check if string starts with character.
   * 
   * @param c Character to check.
   * @return true String starts with required character.
   * @return false String doesn't start with required character.
   */
  bool starts_with(const char &c) const;

  /**
   * @brief Check if string starts with string.
   * 
   * @param c String to check.
   * @return true String starts with required string.
   * @return false String doesn't start with required string.
   */
  bool starts_with(const char *string) const;

  /**
   * @brief Check if string ends with character.
   * 
   * @param c Character to check.
   * @return true String ends with required character.
   * @return false String doesn't end with required character.
   */
  bool ends_with(const char &c) const;

  /**
   * @brief Check if string ends with string.
   * 
   * @param c String to check.
   * @return true String ends with required string.
   * @return false String doesn't end with required string.
   */
  bool ends_with(const char *string) const;

  /**
   * @brief Check if this string is equal to the another one.
   * 
   * @param string String to check.
   * @return true Strings are equal.
   * @return false Strings aren't equal.
   */
  bool equals(const SimpleString &string) const;

  /**
   * @brief Check if this string is equal to the another one.
   * 
   * @param string String to check.
   * @return true Strings are equal.
   * @return false Strings aren't equal.
   */
  bool equals(const char *string) const;

  /**
   * @brief Transform characters to upper-case.
   */
  void to_upper();

  /**
   * @brief Transform characters to lower-case.
   */
  void to_lower();

  /**
   * @brief Replace all old characters with new characters.
   * 
   * @param oldChar Charater to replace.
   * @param newChar Replacement charater.
   */
  void replace(const char &oldChar, const char &newChar);

  /**
   * @brief Replace all old substrings with new strings.
   * 
   * @param oldChar Substring to replace.
   * @param newChar Replacement string.
   */
  void replace(const char *oldString, const char *newString);

  /**
   * @brief Remove all occurrencies of character.
   * 
   * @param c Character to remove.
   */
  void remove(const char &c);

  /**
   * @brief Remove all occurrencies of substring.
   * 
   * @param c Substring to remove.
   */
  void remove(const char *oldString);

  /**
   * @brief Create substring from given index to end of the string.
   * 
   * @param fromIndex Index from which to create substring.
   * @return SimpleString Substring from given index.
   */
  SimpleString substring(const size_t fromIndex) const;

  /**
   * @brief Create substring from given index with length.
   * 
   * @param fromIndex Index from which to create substring
   * @param length Length of substring.
   * @return SimpleString Substring from given index, with length.
   */
  SimpleString substring(const size_t fromIndex, const size_t length) const;

  /**
   * @brief Split string by separator to multiple strings.
   * 
   * @param separator Separator.
   * @return std::vector<SimpleString> Vector of strings.
   */
  std::vector<std::shared_ptr<SimpleString>> split(const char &separator) const;

  /**
   * @brief Conversion to c string, char array.
   * 
   * @return const char* C like string
   */
  operator const char *() const;

  /**
   * @brief Character indexing.
   * 
   * @param index Index of character in string.
   * @return char& Reference to character.
   */
  char &operator[](const int &index);

  void operator=(const char *cString);
  bool operator==(const char *cString) const;
  bool operator==(const SimpleString &string) const;
  void operator+=(const char *cString);
  SimpleString operator+(const char *cString) const;

  /**
   * @brief Create string by replicating.
   * 
   * @param cString String to replicate.
   * @param replicationCount Number of replications.
   * @return SimpleString Replicated string.
   */
  static SimpleString replicate(const char *cString, const int replicationCount);
};
} // namespace azgra
