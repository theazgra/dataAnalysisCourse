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
    size_t m_attributeCount;
    size_t m_targetAttributeIndex;

    void finish_initialization();

public:
    DataFrame() = default;

    DataFrame(DataFrame &&moveSrc) noexcept;

    DataFrame &operator=(DataFrame &&other) noexcept = default;

    void print_header() const;

    void min_max_scaling(double min, double max);

    double operator()(const size_t transactionId, const size_t column) const;

    [[nodiscard]] std::vector<double> get_column(size_t colIndex) const;

    [[nodiscard]] azgra::Matrix<double> const &get_data() const;

    [[nodiscard]] size_t get_attribute_count() const;

    [[nodiscard]] size_t get_target_attribute_index() const;

    [[nodiscard]] size_t size() const;

    azgra::Matrix<double> const& matrix() const;
};
