#include "csv_data_column.h"

CsvDataColumn::CsvDataColumn(DataColumnType columnType, const std::string &columnName, const size_t rowCount)
{
    m_type = columnType;
    m_values.resize(rowCount);
    m_name = columnName;
}

std::vector<std::vector<double>> CsvDataColumn::convert_to_data_columns()
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

std::vector<std::vector<double>> CsvDataColumn::numerical_column_to_data_column()
{
    // Just convert string values to doubles.
    const auto columnValues = azgra::collection::select(m_values.begin(), m_values.end(), [](const std::string &value)
    {
        return std::stod(value);
    });

    return {columnValues};
}

std::vector<std::vector<double>> CsvDataColumn::categorical_column_to_data_column()
{
    // Binarization of values.
    const auto uniqueValues = azgra::collection::distinct(m_values.begin(), m_values.end());
    std::vector<std::vector<double>> columns(uniqueValues.size(), std::vector<double>(m_values.size(), 0.0));

    for (size_t rowIndex = 0; rowIndex < m_values.size(); ++rowIndex)
    {
        const auto columnIndex = azgra::collection::find_index(uniqueValues.begin(), uniqueValues.end(), m_values[rowIndex]);
        columns[columnIndex][rowIndex] = 1.0;
    }
    return columns;
}

std::vector<std::vector<double>> CsvDataColumn::categorical_ordinal_column_to_data_column()
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

    return {columnValues};
}

void CsvDataColumn::move_value(const size_t row, std::string &value)
{
    m_values[row] = std::move(value);
}


