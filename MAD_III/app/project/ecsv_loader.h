#pragma once

#include "csv_data_column.h"
#include <azgra/io/text_file_functions.h>


class ECsvLoader
{
public:
    static std::vector<CsvDataColumn> load_csv_columns(const char *inputFile, bool skipRowWithMissingField);
};