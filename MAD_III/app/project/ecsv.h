#pragma once

#include "csv_data_column.h"
#include "dataframe.h"

class ECsv
{
private:
    std::vector<CsvDataColumn> m_columns;
    size_t m_transactionCount;
public:
    explicit ECsv(std::vector<CsvDataColumn> &columns);

    [[nodiscard]] DataFrame convert_to_dataframe() const;
};
