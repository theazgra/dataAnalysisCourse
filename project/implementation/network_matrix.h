#pragma once
#include "libs.h"

class NetworkMatrix
{
  private:
    std::vector<float> data;
    uint rowCount;
    uint colCount;

    std::vector<uint> get_neighbours(const uint vertex) const;
    uint get_edge_count_between_neighbours(const std::vector<uint> &neighbours) const;

  public:
    NetworkMatrix(uint rowCount, uint colCount);
    ~NetworkMatrix();

    uint rows() const;
    uint cols() const;
    uint vertex_count() const;
    uint edge_count() const;

    void insert(const NetworkMatrix &other);
    void set_matrix_to_one_value(const float value);
    void transpose();
    std::vector<uint> get_degree_of_vertices() const;
    float get_average_degree() const;
    void set_inf_where_no_edge();
    NetworkMatrix get_distance_matrix() const;
    float get_average_clustering_coefficient() const;
    std::vector<float> get_clustering_coeff_for_vertices(const std::vector<uint> &vertices) const;
    std::vector<float> get_clustering_coeff_for_all_vertices() const;
    std::map<uint, std::vector<uint>> get_vertices_grouped_by_degree() const;

    //closeness_centrality_for_vertices

    //
    //save_clustering_coefficient_for_degree_groups

    void copy_data(const NetworkMatrix &source);
    inline float &at(uint row, uint col);
    inline const float &at(uint row, uint col) const;
    const bool is_infinity(uint row, uint col) const;

    NetworkMatrix operator+(const NetworkMatrix &other);
    NetworkMatrix &operator+=(const NetworkMatrix &other);

    NetworkMatrix operator-(const NetworkMatrix &other);
    NetworkMatrix &operator-=(const NetworkMatrix &other);

    void load_from_edges(const std::vector<std::pair<uint, uint>> &edges, int offset = -1);
    void print_matrix() const;
    void export_network(const char *filename) const;
};

#include "network_matrix.cpp"