#include "ecsv_loader.h"

static DataColumnType column_type(const std::string &cellValue)
{
    if (cellValue == "N")
        return DataColumnType::DataColumnType_Numerical;
    if (cellValue == "CB")
        return DataColumnType::DataColumnType_Categorical;
    if (cellValue == "CO")
        return DataColumnType::DataColumnType_CategoricalOrdinal;
    throw std::runtime_error("Unknown column type");
}

std::vector<CsvDataColumn> ECsvLoader::load_csv_columns(const char *inputFile, bool skipRowWithMissingField)
{
    auto csvCells = azgra::io::read_csv_cells(inputFile, ",", skipRowWithMissingField);
    if (csvCells.size() < 4)
        return {};

    const auto magic = csvCells[0][0];
    always_assert(magic == "ECSVFILE");
    const auto columnTypes = azgra::collection::select(csvCells[1].begin(), csvCells[1].end(), column_type);
    const size_t columnCount = csvCells[2].size();
    always_assert(columnTypes.size() == columnCount);
    const size_t rowCount = csvCells.size() - 3;

    std::vector<CsvDataColumn> columns(columnCount);

    for (size_t i = 0; i < columnCount; ++i)
    {
        columns[i] = CsvDataColumn(columnTypes[i], csvCells[2][i], rowCount);
    }

    for (size_t row = 0; row < rowCount; ++row)
    {
        for (size_t col = 0; col < columnCount; ++col)
        {
            columns[col].move_value(row, csvCells[3 + row][col]);
        }
    }

    return columns;
}
