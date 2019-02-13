#pragma once
#include "libs.h"

class NetworkMatrix
{
private:
  std::vector<float> data;
  uint rowCount;
  uint colCount;

  bool equals(const NetworkMatrix &other);

  // Reinitialize this matrix to new dimenstions and fill it with zeros.
  void reinitialize(const uint &rowCount, const uint &colCount);

  // Get element at row and col in custom vector.
  inline float &at_vec(const std::vector<float> &vec, const uint &row, const uint &col);

  // Get neighbours of vertex.
  std::vector<uint> get_neighbours(const uint vertex) const;

  // Get edge count between neighbours.
  uint get_edge_count_between_neighbours(const std::vector<uint> &neighbours) const;

  // Find shortest path between source and dest vertex using dijkstra.
  float dijkstra_path(const NetworkMatrix &mat, const uint &source, const uint &dest) const;

  // Check if we can use BFS instead of dijkstra.
  bool can_use_bfs() const;

  // Find shortest path between source and dest vertex using breadth first search.
  float bfs_path(const NetworkMatrix &mat, const uint &source, const uint &dest) const;
  std::vector<uint> find_k_neighbors(const uint row, const uint k);

public:
  NetworkMatrix(const uint &rowCount, const uint &colCount);
  ~NetworkMatrix();

  // Get number of rows.
  uint rows() const;

  // Get number of cols.
  uint cols() const;

  // Get number of vertices.
  uint vertex_count() const;

  // Get number of edges.
  uint edge_count() const;

  // Insert matrix into this matrix.
  void insert(const NetworkMatrix &other);

  // Set all matrix values to one.
  void set_matrix_to_one_value(const float value);

  // Get degree of vertices. Return degrees vector.
  std::vector<uint> get_degree_of_vertices() const;

  // Get average degree of vertices.
  float get_average_degree() const;

  // Set value of INIFINITY everywhere, where is not an edge.
  void set_inf_where_no_edge();

  // Get matrix of distances from vertex to every other one.
  NetworkMatrix get_distance_matrix(const bool forceDijkstra = false) const;

  // Get average clustering coefficient.
  float get_average_clustering_coefficient() const;

  // Get clusteging coefficient for some vertices.
  std::vector<float> get_clustering_coeff_for_vertices(const std::vector<uint> &vertices) const;

  // Get clusteging coefficient for all vertices.
  std::vector<float> get_clustering_coeff_for_all_vertices() const;

  // Get groups of vertices, grouped by its degree.
  std::map<uint, std::vector<uint>> get_vertices_grouped_by_degree() const;

  // Get closeness centrality for all vertices.
  std::vector<float> get_closeness_centrality_for_vertices(const NetworkMatrix &distanceMatrix) const;

  // Copy data from source matrix.
  void copy_data(const NetworkMatrix &source);

  // Get element reference at row and col.
  inline float &at(const uint &row, const uint &col);

  // Get constant element reference at row and col.
  inline const float &at(const uint &row, const uint &col) const;

  // Check if value at row and col is INFINITY.
  inline bool is_infinity(const uint &row, const uint &col) const;

  NetworkMatrix operator+(const NetworkMatrix &other);
  NetworkMatrix &operator+=(const NetworkMatrix &other);

  NetworkMatrix operator-(const NetworkMatrix &other);
  NetworkMatrix &operator-=(const NetworkMatrix &other);
  bool operator==(const NetworkMatrix &other);
  bool operator!=(const NetworkMatrix &other);

  // Calculate edge probability for symmetric random network.
  float get_probability_for_symmetric_network(const uint vertexCount) const;

  // Generate random network in this matrix.
  void generate_random_network(const float edgeProbability, bool autoProbability = true);

  // Generate scale free network in this matrix.
  void generate_scale_free_network(const uint numberOfConnections, const uint numberOfVerticesToAdd);

  // Load matrix from edges.
  void load_from_edges(const std::vector<std::pair<uint, uint>> &edges, int offset = -1);

  // Print matrix to stdout.
  void print_matrix() const;

  // Export network to file.
  void export_network(const char *filename) const;

  void complete_analysis(const char *networkName, const char *filename, const bool verbose = false, const bool complete = true) const;

  float get_network_longest_distance(const NetworkMatrix &distanceMatrix) const;
  float get_network_average_distance(const NetworkMatrix &distanceMatrix) const;
  std::vector<float> get_eccentricities(const NetworkMatrix &distanceMatrix) const;

  void filter_e_radius(const float radius);
  void filter_kNN(const uint k);
  void filter_combinataion_e_knn(const float radius, const uint k);
};

#include "network_matrix.cpp"