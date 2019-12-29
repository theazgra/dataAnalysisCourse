#pragma once

#include <azgra/matrix.h>
#include <azgra/collection/enumerable_functions.h>
#include <sstream>

class DataFrame
{
    friend class ECsv;
private:
    std::vector<std::string> m_columnNames;
    azgra::Matrix<double> m_values;
public:
    DataFrame() = default;
    void print_header() const;

    void min_max_scaling(double min, double max);
    [[nodiscard]] std::vector<double> get_column(size_t colIndex) const;

    [[nodiscard]] azgra::Matrix<double> const& get_data() const;
};
