#pragma once

#include <azgra/io/text_file_functions.h>
#include "ecsv.h"


class ECsvLoader
{
private:
    const static size_t ECSV_MAGIC_ROW = 0;
    const static size_t ECSV_COLUMN_TYPE_ROW = 1;
    const static size_t ECSV_COLUMN_NAME_ROW = 2;
public:

    [[nodiscard]] static ECsv load_ecsv_file(const char *inputFile, bool skipRowWithMissingField);
};