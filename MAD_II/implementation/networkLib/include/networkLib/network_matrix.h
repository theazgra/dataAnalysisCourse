#pragma once

#include <networkLib/cluster.h>
#include <networkLib/graph_component.h>
#include <networkLib/network_report.h>
#include <networkLib/io.h>
#include <networkLib/Stopwatch.h>

namespace azgra::networkLib
{
    struct SwapInfo
    {
        uint aIndex;
        uint bIndex;
        uint cut;

        SwapInfo(uint _a, uint _b, uint _cut) : aIndex(_a), bIndex(_b), cut(_cut)
        {
        }

        bool operator>(const SwapInfo &other)
        { return this->cut > other.cut; }

        bool operator>=(const SwapInfo &other)
        { return this->cut >= other.cut; }

        bool operator<(const SwapInfo &other)
        { return this->cut < other.cut; }

        bool operator<=(const SwapInfo &other)
        { return this->cut <= other.cut; }
    };

    class NetworkMatrix
    {
    private:
        // Matrix data.
        std::vector<float> data;
        // Flags for deleted vertices.
        std::vector<bool> deleted;
        // Row count in matrix.
        uint rowCount;
        // Col count in matrix.
        uint colCount;

        // Check if other network is equal to this one.
        bool equals(const NetworkMatrix &other);

        // Reinitialize this matrix to new dimenstions and fill it with zeros.
        void reinitialize(const uint &rowCount, const uint &colCount);

        // Get element at row and col in custom vector.
        float &at_vec(const std::vector<float> &vec, const uint &row, const uint &col);

        // Get neighbours of vertex which aren't in `except` vector
        std::vector<uint> get_neighbors_except(const uint vertex, const std::vector<uint> &except) const;

        // Get edge count between neighbours.
        uint get_edge_count_between_neighbours(const std::vector<uint> &neighbours) const;

        void dijkstra_distance(const NetworkMatrix &mat, const uint &source, NetworkMatrix &result) const;

        void dijkstra_distance_with_path(const NetworkMatrix &mat, const uint &source, NetworkMatrix &result,
                                         std::vector<std::vector<UndirectedEdge>> &paths) const;

        NetworkMatrix get_cosine_similarity_matrix() const;

        std::vector<Cluster>
        find_clusters_hierarchical(const uint clusterCount, LinkageType linkType, bool calcalateCosineSim = true) const;

        void remove_edges_outside_clusters(const std::vector<Cluster> &clusters);

        void initialize_deleted();

    public:
        NetworkMatrix();

        NetworkMatrix(const NetworkMatrix &copySrc);

        NetworkMatrix(const char *fName, const char *separator = ";", const bool preprocessIds = false);

        NetworkMatrix(const std::vector<std::pair<uint, uint>> &edges, int offset);

        NetworkMatrix(const uint &rowCount, const uint &colCount);

        //NetworkMatrix(const std::vector<IrisRecord> &vectorData);
        ~NetworkMatrix();

        std::vector<std::pair<uint, uint>> get_edges() const;

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
        NetworkMatrix get_distance_matrix() const;

        NetworkMatrix get_edge_betweenness_matrix() const;

        void export_edge_betweenness(const char *fileName) const;

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
        float &at(const uint &row, const uint &col);

        // Get element reference at index.
        float &at(const uint &index);

        // Get constant element reference at row and col.
        const float &at(const uint &row, const uint &col) const;

        // Get constant element reference at index.
        const float &at(const uint &index) const;

        bool is_edge_at(const uint &row, const uint &col) const;

        // Check if value at row and col is INFINITY.
        bool is_infinity(const uint &row, const uint &col) const;

        NetworkMatrix operator+(const NetworkMatrix &other);

        NetworkMatrix &operator+=(const NetworkMatrix &other);

        NetworkMatrix operator-(const NetworkMatrix &other);

        NetworkMatrix &operator-=(const NetworkMatrix &other);

        bool operator==(const NetworkMatrix &other);

        bool operator!=(const NetworkMatrix &other);

        uint count_in_e_radius(const uint row, const float e) const;

        std::vector<uint> find_k_neighbors(const uint row, const uint k) const;

        void filter_knn_row(const uint row, const uint k);

        // Delete all edges from and to this vertex.
        void delete_edges_for(const uint vertex);

        // Get number of edges between two groups.
        uint get_edge_count_between_groups(const std::vector<uint> &gA, const std::vector<uint> &gB) const;

        uint get_edge_count_in_component(const std::vector<uint> &c);

        uint get_total_degree_of_community(const std::vector<uint> &c);

        // Load matrix from edges.
        void load_from_edges(const std::vector<std::pair<uint, uint>> &edges, int offset = -1);

        // Print matrix to stdout.
        void print_matrix() const;

        // Export network to file.
        void export_network(const char *filename, bool allSelfEdge = false) const;

        float get_network_diameter(const NetworkMatrix &distanceMatrix) const;

        float get_average_distance(const NetworkMatrix &distanceMatrix) const;

        std::vector<float> get_eccentricities(const NetworkMatrix &distanceMatrix) const;

        void hierarchical_clustering(const uint clusterCount, const char *reportFile, LinkageType linkType, bool calculateCosineSim = true);

        std::vector<GraphComponent> get_components() const;

        void print_network_stats(const char *header) const;

        void failure_step();

        void attack_step();

        NetworkReport get_network_report(const ReportRequest &request) const;

        // Get number of shared neighbors.
        uint get_count_of_shared_neighbors(const uint u, const uint v) const;

        // Get number of shared neighbors.
        uint get_count_of_same_neighbors(const std::vector<uint> &aNeighbors, const std::vector<uint> &bNeighbors) const;
    };
}; // namespace azgra::networkLib