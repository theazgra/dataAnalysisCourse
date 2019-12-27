#pragma once

#include <azgra/azgra.h>
#include <azgra/collection/enumerable_functions.h>

enum DataColumnType
{
    DataColumnType_Numerical,
    DataColumnType_CategoricalOrdinal,
    DataColumnType_Categorical,
};


class CsvDataColumn
{
private:
    DataColumnType m_type = DataColumnType_Numerical;
    std::string m_name;
    std::vector<std::string> m_values{};

    [[nodiscard]] std::vector<std::vector<double>> numerical_column_to_data_column();

    [[nodiscard]] std::vector<std::vector<double>> categorical_column_to_data_column();

    [[nodiscard]] std::vector<std::vector<double>> categorical_ordinal_column_to_data_column();

public:
    CsvDataColumn() = default;

    explicit CsvDataColumn(DataColumnType columnType, const std::string &columnName, const size_t rowCount);

    [[nodiscard]] std::vector<std::vector<double>> convert_to_data_columns();

    void move_value(const size_t row, std::string &value);
};