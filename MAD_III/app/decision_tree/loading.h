#pragma once

#include "dataset.h"
#include <azgra/io/text_file_functions.h>

namespace DecisionTree
{

    Dataset<float> load_numerical(const char *fileName, const char separator, const size_t numberOfAttributes);
    Dataset<char> load_categorial(const char *fileName, const char separator,  const size_t numberOfAttributes);

}