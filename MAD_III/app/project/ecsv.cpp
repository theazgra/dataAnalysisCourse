#include "ecsv.h"

ECsv::ECsv(std::vector<CsvDataColumn> &columns)
{
    m_columns = std::move(columns);

    m_transactionCount = m_columns[0].transaction_count();
}

DataFrame ECsv::convert_to_dataframe() const
{
    DataFrame df;
    const size_t dfColumnCount = azgra::collection::sum(m_columns.begin(), m_columns.end(),
                                                        [](const CsvDataColumn &c)
                                                        { return c.data_column_count(); }, 0);

    df.m_columnNames.resize(dfColumnCount);
    auto dfMat = azgra::Matrix<double>(m_transactionCount, dfColumnCount, 0.0);
    df.m_values = std::move(dfMat);

    size_t dataColIndex = 0;
    size_t dataColNameIndex = 0;
    for (const CsvDataColumn &col : m_columns)
    {
        const auto[dataColumns, dataColumnNames] = col.convert_to_data_columns();

        for (const auto &dataColumn : dataColumns)
        {
            always_assert(dataColumn.size() == m_transactionCount);
            df.m_values.col(dataColIndex++, dataColumn.begin(), dataColumn.end());
        }

        for (const auto &colName : dataColumnNames)
        {
            df.m_columnNames[dataColNameIndex++] = colName;
        }
    }

    // Last column is target.
    df.m_targetAttributeIndex = m_columns.size() - 1;

    df.finish_initialization();

    return df;
}
