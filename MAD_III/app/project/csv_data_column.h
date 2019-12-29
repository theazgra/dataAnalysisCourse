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
    std::vector<std::string> m_uniqueValues{};

    [[nodiscard]] std::pair<std::vector<std::vector<double>>, std::vector<std::string>> numerical_column_to_data_column() const;

    [[nodiscard]] std::pair<std::vector<std::vector<double>>, std::vector<std::string>> categorical_column_to_data_column() const;

    [[nodiscard]] std::pair<std::vector<std::vector<double>>, std::vector<std::string>> categorical_ordinal_column_to_data_column() const;

public:
    CsvDataColumn() = default;

    explicit CsvDataColumn(DataColumnType columnType, const std::string &columnName, const size_t rowCount);

    [[nodiscard]] std::pair<std::vector<std::vector<double>>, std::vector<std::string>> convert_to_data_columns() const;

    [[nodiscard]] size_t data_column_count() const;

    [[nodiscard]] size_t transaction_count() const;

    void move_value(size_t row, std::string &value);

    [[nodiscard]] const std::string &column_name() const;

    void initialize_unique_values();
};