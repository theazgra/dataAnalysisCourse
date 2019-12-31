#include "dataframe.h"

DataFrame::DataFrame(DataFrame &&moveSrc) noexcept
{
    m_columnNames = std::move(moveSrc.m_columnNames);
    m_values = std::move(moveSrc.m_values);
    m_attributeCount = moveSrc.m_attributeCount;
    m_targetAttributeIndex = moveSrc.m_targetAttributeIndex;
}

void DataFrame::print_header() const
{
    std::stringstream ss;
    for (const auto &colName : m_columnNames)
    {
        ss << colName << '|';
    }
    fprintf(stdout, "%s\n", ss.str().c_str());
}


void DataFrame::min_max_scaling(const double min, const double max)
{
    const size_t colCount = m_values.cols();
    std::vector<double> mins(colCount, std::numeric_limits<double>::max());
    std::vector<double> maxes(colCount, std::numeric_limits<double>::min());

    for (size_t row = 0; row < m_values.rows(); ++row)
    {
        for (size_t col = 0; col < colCount; ++col)
        {
            const auto &val = m_values.at(row, col);
            if (val < mins[col])
            {
                mins[col] = val;
            }

            if (val > maxes[col])
            {
                maxes[col] = val;
            }
        }
    }


    for (size_t row = 0; row < m_values.rows(); ++row)
    {
        for (size_t col = 0; col < colCount; ++col)
        {
            if (mins[col] == maxes[col])
            {
                continue;
            }
            if (m_values.at(row, col) != 0)
            {
                const double x_std = (m_values.at(row, col) - mins[col]) / (maxes[col] - mins[col]);
                m_values.at(row, col) = (x_std * (max - min)) + min;
            }
        }
    }
}


std::vector<double> DataFrame::get_column(size_t colIndex) const
{
    return m_values.col(colIndex);
}

azgra::Matrix<double> const &DataFrame::get_data() const
{
    return m_values;
}

size_t DataFrame::get_attribute_count() const
{
    return m_attributeCount;
}

void DataFrame::finish_initialization()
{
    m_attributeCount = m_values.cols();
}

size_t DataFrame::get_target_attribute_index() const
{
    return m_targetAttributeIndex;
}

size_t DataFrame::size() const
{
    return m_values.rows();
}

double DataFrame::operator()(const size_t transactionId, const size_t column) const
{
    return m_values.at(transactionId, column);
}
