using NetworkWizardLib.NWRandom;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace NetworkWizardLib.NWNetwork
{
    public class UndirectedNetworkMatrix : IEquatable<UndirectedNetworkMatrix>
    {
        public float[,] Data { get; private set; }

        public float this[int row, int col] { get { return Data[row, col]; } set { Data[row, col] = value; } }

        public bool[] DeletedVertices { get; private set; }

        public int Dimension { get; private set; } = 0;

        public UndirectedNetworkMatrix(int dimension)
        {
            Dimension = dimension;
            Data = new float[Dimension, Dimension];
            DeletedVertices = new bool[Dimension];
        }

        public UndirectedNetworkMatrix(UndirectedNetworkMatrix other)
        {
            Dimension = other.Dimension;
            Data = new float[Dimension, Dimension];
            DeletedVertices = new bool[Dimension];
            Insert(other);
        }

        public override bool Equals(object obj)
        {
            if (obj is UndirectedNetworkMatrix other)
                return Equals(other);
            else
                return base.Equals(obj);
        }

        public bool Equals(UndirectedNetworkMatrix other)
        {
            if (Dimension != other.Dimension)
                return false;

            for (int row = 0; row < Dimension; row++)
            {
                for (int col = 0; col < Dimension; col++)
                {
                    if (Data[row, col] != other.Data[row, col])
                        return false;
                }
            }
            return true;
        }

        public static UndirectedNetworkMatrix operator +(UndirectedNetworkMatrix a, UndirectedNetworkMatrix b)
        {
            Trace.Assert(a.Dimension == b.Dimension);
            UndirectedNetworkMatrix result = new UndirectedNetworkMatrix(a.Dimension);

            for (int row = 0; row < a.Dimension; row++)
            {
                for (int col = 0; col < a.Dimension; col++)
                {
                    result.Data[row, col] = a.Data[row, col] + b.Data[row, col];
                }
            }

            return result;
        }

        public static UndirectedNetworkMatrix operator -(UndirectedNetworkMatrix a, UndirectedNetworkMatrix b)
        {
            Trace.Assert(a.Dimension == b.Dimension);
            UndirectedNetworkMatrix result = new UndirectedNetworkMatrix(a.Dimension);

            for (int row = 0; row < a.Dimension; row++)
            {
                for (int col = 0; col < a.Dimension; col++)
                {
                    result.Data[row, col] = a.Data[row, col] - b.Data[row, col];
                }
            }

            return result;
        }

        public void SetToValue(float value = 0.0f)
        {
            for (int row = 0; row < Dimension; row++)
            {
                for (int col = row; col < Dimension; col++)
                {
                    Data[row, col] = value;
                    Data[col, row] = value;
                }
            }
        }

        private void SetInfinityWhereIsNoEdge()
        {
            for (int row = 0; row < Dimension; row++)
            {
                for (int col = row; col < Dimension; col++)
                {
                    if (Data[row, col] <= 0.0f)
                    {
                        Data[row, col] = float.PositiveInfinity;
                        Data[col, row] = float.PositiveInfinity;
                    }
                }
            }
        }

        public void Reinitialize(int dimension)
        {
            Dimension = dimension;
            Data = new float[Dimension, Dimension];
            DeletedVertices = new bool[Dimension];
            SetToValue(0.0f);
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(Data, Dimension);
        }

        public void Insert(UndirectedNetworkMatrix other)
        {
            Trace.Assert(other.Dimension <= Dimension);
            for (int row = 0; row < other.Dimension; row++)
            {
                for (int col = 0; col < other.Dimension; col++)
                {
                    Data[row, col] = other.Data[row, col];
                }
            }
        }

        private bool IsInfinityAt(int row, int col) => (Data[row, col] == float.PositiveInfinity);
        private bool IsEdgeAt(int row, int col) => (Data[row, col] > 0.0f && !IsInfinityAt(row, col));

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();

            for (int row = 0; row < Dimension; row++)
            {
                for (int col = 0; col < Dimension; col++)
                {
                    sb.Append($"{Data[row, col]} ");
                }
                sb.AppendLine();
            }
            return sb.ToString();
        }

        public IEnumerable<ValueTuple<int, int>> GetEdges(bool forceSelfEdges = false)
        {
            List<ValueTuple<int, int>> edges = new List<(int, int)>();
            for (int row = 0; row < Dimension; row++)
            {
                if (DeletedVertices[row])
                    continue;

                if (forceSelfEdges)
                    edges.Add((row, row));

                for (int col = row + 1; col < Dimension; col++)
                {
                    if (DeletedVertices[col])
                        continue;

                    if (IsEdgeAt(row, col))
                        edges.Add((row, col));
                }
            }
            return edges;
        }

        internal IEnumerable<int> GetNeighbors(int vertex, IEnumerable<int> except = null)
        {
            if (DeletedVertices[vertex])
                throw new Exception($"{vertex} is deleted vertex!");

            List<int> neighbors = new List<int>();

            for (int neighbor = 0; neighbor < Dimension; neighbor++)
            {
                if (neighbor == vertex || DeletedVertices[neighbor])
                    continue;

                if (IsEdgeAt(vertex, neighbor))
                {
                    if (except != null)
                    {
                        if (except.Contains(neighbor))
                            continue;
                    }
                    neighbors.Add(neighbor);
                }
            }
            return neighbors;
        }

        public int VertexCount() => DeletedVertices.Count(v => !v);

        public int EdgeCount()
        {
            int result = 0;
            for (int row = 0; row < Dimension; row++)
            {
                if (DeletedVertices[row])
                    continue;
                for (int col = row + 1; col < Dimension; col++)
                {
                    if (!DeletedVertices[col] && IsEdgeAt(row, col))
                        ++result;
                }
            }
            return result;
        }

        internal IEnumerable<KeyValuePair<int, int>> GetDegreeOfVertices()
        {
            int expected = VertexCount();
            List<KeyValuePair<int, int>> result = new List<KeyValuePair<int, int>>(expected);

            for (int vertex = 0; vertex < Dimension; vertex++)
            {
                if (DeletedVertices[vertex])
                    continue;

                int degree = 0;

                for (int neighbor = 0; neighbor < Dimension; neighbor++)
                {
                    if (!DeletedVertices[neighbor] && IsEdgeAt(vertex, neighbor))
                        degree++;
                }
                result.Add(new KeyValuePair<int, int>(vertex, degree));
            }

            Trace.Assert(result.Count == expected);
            return result;
        }

        public UndirectedNetworkMatrix GetDistanceMatrix()
        {
            bool useBfs = CanUseBfs();
            UndirectedNetworkMatrix result = new UndirectedNetworkMatrix(Dimension);

            UndirectedNetworkMatrix distanceMatrix = new UndirectedNetworkMatrix(this);
            distanceMatrix.SetInfinityWhereIsNoEdge();

            //for (int row = 0; row < Dimension; row++)
            Parallel.ForEach(Enumerable.Range(0, Dimension), row =>
            {
                if (!DeletedVertices[row])
                {
                    for (int col = row + 1; col < Dimension; col++)
                    {
                        if (DeletedVertices[col])
                            continue;

                        float distance = useBfs ? PathFinding.BfsPath(distanceMatrix, row, col) : PathFinding.DijkstraPath(distanceMatrix, row, col);
                        result.Data[row, col] = distance;
                        result.Data[col, row] = distance;
                    }
                }
            });


            return result;
        }

        private bool CanUseBfs()
        {
            for (uint row = 0; row < Dimension; row++)
            {
                for (uint col = row + 1; col < Dimension; col++)
                {
                    if (Data[row, col] > 1.0f || Data[col, row] > 1.0f)
                        return false;
                }
            }
            return true;
        }
    }
}
/*
    
  uint get_count_of_same_neighbors(const std::vector<uint> &aNeighbors, const std::vector<uint> &bNeighbors) const;
  

  uint get_edge_count_between_neighbours(const std::vector<uint> &neighbours) const;
  float bfs_path(const NetworkMatrix &mat, const uint &source, const uint &dest) const;
  float dijkstra_path(const NetworkMatrix &mat, const uint &source, const uint &dest) const;
  std::vector<uint> find_k_neighbors(const uint row, const uint k) const;
  uint count_in_e_radius(const uint row, const float e) const;
  void filter_knn_row(const uint row, const uint k);
  uint get_edge_count_between_groups(const std::vector<uint> &gA, const std::vector<uint> &gB) const;
  void delete_edges_for(const uint vertex);
  NetworkMatrix get_initial_matrix_of_size_3() const;
  NetworkMatrix get_cosine_similarity_matrix() const;void remove_edges_outside_clusters(const std::vector<Cluster> &clusters);
  std::vector<std::pair<uint, uint>> get_edges() const;
  NetworkMatrix(const NetworkMatrix &copySrc);
  NetworkMatrix(const char *fName);
  NetworkMatrix(const uint &rowCount, const uint &colCount);

  uint get_vertex_with_max_degree() const;
  float get_average_degree() const;

  

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

  void failure_step();
  void attack_step();
     */
