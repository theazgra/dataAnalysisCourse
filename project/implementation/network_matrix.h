#pragma once
#include "libs.h"

// struct DijkstraVertex
// {
//   uint vertex;
//   float distance;
//   bool visited = false;
//   bool deleted = false;
//   bool hasNeighSetAlready = false;
//   std::vector<DijkstraVertex *> neighbours;

//   bool operator==(const DijkstraVertex &o)
//   {
//     return (this->vertex == o.vertex);
//   }
// };

class NetworkMatrix
{
private:
  std::vector<float> data;
  uint rowCount;
  uint colCount;

  void reinitialize(const uint &rowCount, const uint &colCount);
  std::vector<uint> get_neighbours(const uint vertex) const;
  uint get_edge_count_between_neighbours(const std::vector<uint> &neighbours) const;
  inline float &at_vec(const std::vector<float> &vec, const uint &row, const uint &col);

public:
  NetworkMatrix(const uint &rowCount, const uint &colCount);
  ~NetworkMatrix();

  uint rows() const;
  uint cols() const;
  uint vertex_count() const;
  uint edge_count() const;

  void insert(const NetworkMatrix &other);
  void set_matrix_to_one_value(const float value);
  std::vector<uint> get_degree_of_vertices() const;
  float get_average_degree() const;
  void set_inf_where_no_edge();
  NetworkMatrix get_distance_matrix() const;
  float get_average_clustering_coefficient() const;
  std::vector<float> get_clustering_coeff_for_vertices(const std::vector<uint> &vertices) const;
  std::vector<float> get_clustering_coeff_for_all_vertices() const;
  std::map<uint, std::vector<uint>> get_vertices_grouped_by_degree() const;

  //closeness_centrality_for_vertices
  void copy_data(const NetworkMatrix &source);
  inline float &at(const uint &row, const uint &col);
  inline const float &at(const uint &row, const uint &col) const;
  inline bool is_infinity(const uint &row, const uint &col) const;

  NetworkMatrix operator+(const NetworkMatrix &other);
  NetworkMatrix &operator+=(const NetworkMatrix &other);

  NetworkMatrix operator-(const NetworkMatrix &other);
  NetworkMatrix &operator-=(const NetworkMatrix &other);

  std::vector<float> dijkstra_paths_for_vertex(const uint &source) const;
  float get_probability_for_symmetric_network(const uint vertexCount) const;
  void generate_random_network(const uint vertexCount, const float edgeProbability, bool autoProbability = true);
  void generate_scale_free_network(const uint numberOfConnections, const uint numberOfVerticesToAdd);

  void load_from_edges(const std::vector<std::pair<uint, uint>> &edges, int offset = -1);
  void print_matrix() const;
  void export_network(const char *filename) const;
};

#include "network_matrix.cpp"