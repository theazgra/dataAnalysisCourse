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

ECsv ECsvLoader::load_ecsv_file(const char *inputFile, bool skipRowWithMissingField)
{

    auto csvCells = azgra::io::read_csv_cells(inputFile, ",", skipRowWithMissingField);
    if (csvCells.size() < 4)
        throw std::runtime_error("Invalid ECSV file.");

    const auto magic = csvCells[ECSV_MAGIC_ROW][0];
    always_assert(magic == "ECSVFILE");
    const auto columnTypes = azgra::collection::select(csvCells[ECSV_COLUMN_TYPE_ROW].begin(), csvCells[ECSV_COLUMN_TYPE_ROW].end(), column_type);
    const size_t columnCount = csvCells[ECSV_COLUMN_NAME_ROW].size();
    always_assert(columnTypes.size() == columnCount);
    const size_t rowCount = csvCells.size() - 3;

    std::vector<CsvDataColumn> columns(columnCount);

    for (size_t i = 0; i < columnCount; ++i)
    {
        columns[i] = CsvDataColumn(columnTypes[i], csvCells[ECSV_COLUMN_NAME_ROW][i], rowCount);
    }

    for (size_t row = 0; row < rowCount; ++row)
    {
        for (size_t col = 0; col < columnCount; ++col)
        {
            columns[col].move_value(row, csvCells[3 + row][col]);
        }
    }
    std::for_each(columns.begin(), columns.end(),[](CsvDataColumn &c){ c.initialize_unique_values(); });

    ECsv ecsv(columns);
    return ecsv;
}
