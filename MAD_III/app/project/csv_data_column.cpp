#include "csv_data_column.h"

CsvDataColumn::CsvDataColumn(DataColumnType columnType, const std::string &columnName, const size_t rowCount)
{
    m_type = columnType;
    m_values.resize(rowCount);
    m_name = columnName;
}


void CsvDataColumn::initialize_unique_values()
{
    if (m_type == DataColumnType::DataColumnType_Categorical)
    {
        m_uniqueValues = azgra::collection::distinct(m_values.begin(), m_values.end());
    }
}


std::pair<std::vector<std::vector<double>>, std::vector<std::string>> CsvDataColumn::convert_to_data_columns() const
{
    switch (m_type)
    {
        case DataColumnType_Numerical:
            return numerical_column_to_data_column();
        case DataColumnType_CategoricalOrdinal:
            return categorical_ordinal_column_to_data_column();
        case DataColumnType_Categorical:
            return categorical_column_to_data_column();
    }
    throw std::runtime_error("Wrong column type");
}

std::pair<std::vector<std::vector<double>>, std::vector<std::string>> CsvDataColumn::numerical_column_to_data_column() const
{
    // Just convert string values to doubles.
    const auto columnValues = azgra::collection::select(m_values.begin(), m_values.end(), [](const std::string &value)
    {
        return std::stod(value);
    });

    return {{columnValues},
            {m_name}};
}

std::pair<std::vector<std::vector<double>>, std::vector<std::string>> CsvDataColumn::categorical_column_to_data_column() const
{
    // Binarization of values.
    std::vector<std::vector<double>> columns(m_uniqueValues.size(), std::vector<double>(m_values.size(), 0.0));
    std::vector<std::string> binColNames(m_uniqueValues.size());

    for (size_t i = 0; i < m_uniqueValues.size(); ++i)
    {
        binColNames[i] = m_name + "_" + m_uniqueValues[i];
    }

    for (size_t rowIndex = 0; rowIndex < m_values.size(); ++rowIndex)
    {
        const auto columnIndex = azgra::collection::find_index(m_uniqueValues.begin(), m_uniqueValues.end(), m_values[rowIndex]);
        columns[columnIndex][rowIndex] = 1.0;
    }
    return {columns, binColNames};
}

std::pair<std::vector<std::vector<double>>, std::vector<std::string>> CsvDataColumn::categorical_ordinal_column_to_data_column() const
{
    //one,two,three,four, five, six, seven,eight, nine,ten, eleven, twelve

    const auto columnValues = azgra::collection::select(m_values.begin(), m_values.end(), [](const std::string &value)
    {
        if (value == "one")
            return 1.0;
        if (value == "two")
            return 2.0;
        if (value == "three")
            return 3.0;
        if (value == "four")
            return 4.0;
        if (value == "five")
            return 5.0;
        if (value == "six")
            return 6.0;
        if (value == "seven")
            return 7.0;
        if (value == "eight")
            return 8.0;
        if (value == "nine")
            return 9.0;
        if (value == "ten")
            return 10.0;
        if (value == "eleven")
            return 11.0;
        if (value == "twelve")
            return 12.0;
        if (value == "thirteen")
            return 13.0;
        if (value == "fourteen")
            return 14.0;
        if (value == "fifteen")
            return 15.0;

        throw std::runtime_error("Unsupported ordinal value");
    });

    return {{columnValues}, {m_name}};
}

void CsvDataColumn::move_value(const size_t row, std::string &value)
{
    m_values[row] = std::move(value);
}

const std::string &CsvDataColumn::column_name() const
{
    return m_name;
}

size_t CsvDataColumn::data_column_count() const
{
    switch (m_type)
    {
        case DataColumnType_Numerical:
        case DataColumnType_CategoricalOrdinal:
            return 1;
        case DataColumnType_Categorical:
            return m_uniqueValues.size();
    }
    throw std::runtime_error("Invalid column type.");
}

size_t CsvDataColumn::transaction_count() const
{
    return m_values.size();
}
