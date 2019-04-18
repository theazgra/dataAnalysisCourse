#pragma once
#include "cluster.h"
#include "graph_component.h"
#include "io.h"

struct ClusterCandidate
{
  Cluster i;
  Cluster j;
  float similarity;

  void set_new(Cluster i, Cluster j, float sim)
  {
    this->i = i;
    this->j = j;
    similarity = sim;
  }
};

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

    // Get neighbours of vertex which aren't in `except`
  std::vector<uint> get_neighbors_except(const uint vertex, const std::vector<uint> &except) const;

  // Get edge count between neighbours.
  uint get_edge_count_between_neighbours(const std::vector<uint> &neighbours) const;

  // Find shortest path between source and dest vertex using dijkstra.
  float dijkstra_path(const NetworkMatrix &mat, const uint &source, const uint &dest) const;

  // Check if we can use BFS instead of dijkstra.
  bool can_use_bfs() const;

  // Find shortest path between source and dest vertex using breadth first search.
  float bfs_path(const NetworkMatrix &mat, const uint &source, const uint &dest) const;
  std::vector<uint> find_k_neighbors(const uint row, const uint k) const;
  uint count_in_e_radius(const uint row, const float e) const;
  void filter_knn_row(const uint row, const uint k);

  // Get number of edges between two groups.
  uint get_edge_count_between_groups(const std::vector<uint> &gA, const std::vector<uint> &gB) const;

  void delete_edges_for(const uint vertex);

  // Get symmetrical network of size 3.
  NetworkMatrix get_initial_matrix_of_size_3() const;

  NetworkMatrix get_cosine_similarity_matrix() const;

  uint get_count_of_same_neighbors(const std::vector<uint> &aNeighbors, const std::vector<uint> &bNeighbors) const;

  std::vector<Cluster> find_clusters_hierarchical(const uint clusterCount, LinkageType linkType) const;
  void remove_edges_outside_clusters(const std::vector<Cluster> &clusters);

  void initialize_deleted();

  //std::vector<uint> deletedVertices;
  std::vector<bool> deleted;
  std::vector<std::pair<uint, uint>> get_edges() const;

public:
  NetworkMatrix(const NetworkMatrix &copySrc);
  NetworkMatrix(const char *fName, const int offset);
  NetworkMatrix(const uint &rowCount, const uint &colCount);
  NetworkMatrix(const std::vector<IrisRecord> &vectorData);
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

  uint get_vertex_with_max_degree() const;

  // Get neighbours of vertex.
  std::vector<uint> get_neighbors(const uint vertex) const;

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

  void generate_scale_free_network(const uint initialSize, const uint finalSize, const uint numberOfConnections);

  // Generate network based on Holme-Kim model.
  void generate_holme_kim(float probability, const uint newVertexConnections);

  // Generate network based on Bianconi model.
  void generate_bianconi(float probability, const uint newVertexConnections);

  void generate_select_link_or_copy(bool copy, float copyProbability);

  // Load matrix from edges.
  void load_from_edges(const std::vector<std::pair<uint, uint>> &edges, int offset = -1);

  // Print matrix to stdout.
  void print_matrix() const;

  // Export network to file.
  void export_network(const char *filename, bool allSelfEdge = false) const;

  void complete_analysis(const char *networkName, const char *filename, const bool verbose = false, const bool complete = true) const;

  float get_network_longest_distance(const NetworkMatrix &distanceMatrix) const;
  float get_network_average_distance(const NetworkMatrix &distanceMatrix) const;
  std::vector<float> get_eccentricities(const NetworkMatrix &distanceMatrix) const;

  // For network created from vector dataset. Filter edges based on epsilon radius.
  void filter_e_radius(const float radius);
  // For network created from vector dataset. Filter edges based on KNN.
  void filter_kNN(const uint k);
  // For network created from vector dataset. Filter edges based on combination of epilon radius and KNN.
  void filter_combinataion_e_knn(const float radius, const uint k);
  // Create sample from this network, with random node sampling method. Sample vertex count is equal to vertex_count() * `targetPercentSize`, which is < 1.0f.
  NetworkMatrix filter_random_node_sampling(const float targetPercentSize) const;
  // Create sample from this network, with random edge sampling method. Sample vertex count is equal to vertex_count() * `targetPercentSize`, which is < 1.0f.
  NetworkMatrix filter_random_edge_sampling(const float targetPercentSize) const;

  void filter_k_core(const uint k);

  void kernighan_lin() const;

  void hierarchical_clustering(const uint clusterCount, const char *reportFile, LinkageType linkType);
  std::vector<GraphComponent> get_components() const;
  void print_network_stats(const char *header) const;

  void failure_step();
  void attack_step();
};

#include "network_matrix.cpp"
