#include <networkLib/network_matrix.h>

void NetworkMatrix::initialize_deleted()
{
    deleted.resize(rowCount);
    for (size_t i = 0; i < rowCount; i++)
        deleted[i] = false;
}

NetworkMatrix::NetworkMatrix()
{
    this->rowCount = 0;
    this->colCount = 0;
}

NetworkMatrix::NetworkMatrix(const char *fName)
{
    int offset;
    auto loadedEdges = load_edge_pairs(fName, ";", offset);
    uint vc = get_vertex_count_from_edge_pairs(loadedEdges);
    this->rowCount = vc;
    this->colCount = vc;

    this->data.resize(this->rowCount * this->colCount);

    set_matrix_to_one_value(0.0f);

    load_from_edges(loadedEdges, offset);
    initialize_deleted();
}

NetworkMatrix::NetworkMatrix(const NetworkMatrix &copySrc)
{
    this->rowCount = copySrc.rows();
    this->colCount = copySrc.cols();
    this->data.resize(this->rowCount * this->colCount);
    copy_data(copySrc);
    initialize_deleted();
}

NetworkMatrix::NetworkMatrix(const uint &rowCount, const uint &colCount)
{
    this->rowCount = rowCount;
    this->colCount = colCount;
    this->data.resize(this->rowCount * this->colCount);
    set_matrix_to_one_value(0.0f);
    initialize_deleted();
}

NetworkMatrix NetworkMatrix::get_initial_matrix_of_size_3() const
{
    NetworkMatrix initialMat(3, 3);
    {
        initialMat.at(0, 0) = 0.0f;
        initialMat.at(0, 1) = 1.0f;
        initialMat.at(0, 2) = 1.0f;

        initialMat.at(1, 0) = 1.0f;
        initialMat.at(1, 1) = 0.0f;
        initialMat.at(1, 2) = 1.0f;

        initialMat.at(2, 0) = 1.0f;
        initialMat.at(2, 1) = 1.0f;
        initialMat.at(2, 2) = 0.0f;
    }
    return initialMat;
}

uint NetworkMatrix::get_count_of_same_neighbors(const std::vector<uint> &aNeighbors, const std::vector<uint> &bNeighbors) const
{
    uint count = 0;
    for (const uint &v : aNeighbors)
    {
        if (find(bNeighbors, v))
            count++;
    }
    return count;
}

NetworkMatrix NetworkMatrix::get_cosine_similarity_matrix() const
{
    std::vector<std::vector<uint>> vertexNeighbors;
    std::vector<uint> vertexDegree = get_degree_of_vertices();

    vertexNeighbors.resize(this->rowCount);
    for (uint vertex = 0; vertex < this->rowCount; vertex++)
    {
        vertexNeighbors[vertex] = get_neighbors(vertex);
    }

    NetworkMatrix cosineMat = NetworkMatrix(rowCount, rowCount);
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = row; col < this->colCount; col++)
        {
            if (row == col)
            {
                cosineMat.at(row, col) = 1.0f;
            }
            else
            {
                uint neighCount = get_count_of_same_neighbors(vertexNeighbors[row], vertexNeighbors[col]);

                float sim = (float)neighCount / sqrt(((float)vertexDegree[row] * (float)vertexDegree[col]));

                cosineMat.at(row, col) = sim;
                cosineMat.at(col, row) = sim;
            }
        }
    }

    return cosineMat;
}

float get_average_linkage_similarity(const NetworkMatrix &cosineMat,
                                     const std::vector<uint> &aVertices,
                                     const std::vector<uint> &bVertices)
{
    float totalCount = aVertices.size() * bVertices.size();
    float sim = 0.0f;

    for (const uint &u : aVertices)
    {
        for (const uint &v : bVertices)
        {
            sim += cosineMat.at(u, v);
        }
    }

    sim /= totalCount;
    return sim;
}

float get_single_linkage_similarity(const NetworkMatrix &cosineMat,
                                    const std::vector<uint> &aVertices,
                                    const std::vector<uint> &bVertices)
{
    // Single linkage means the closest vertices, so similarity must be maximal.
    float sim = -999999;

    for (const uint &u : aVertices)
    {
        for (const uint &v : bVertices)
        {
            if (cosineMat.at(u, v) > sim)
            {
                sim = cosineMat.at(u, v);
            }
        }
    }
    return sim;
}

float get_complete_linkage_similarity(const NetworkMatrix &cosineMat,
                                      const std::vector<uint> &aVertices,
                                      const std::vector<uint> &bVertices)
{
    // Complete linkage means the furthest vertices, so similarity must be minimal.
    float sim = 999999;

    for (const uint &u : aVertices)
    {
        for (const uint &v : bVertices)
        {
            if (cosineMat.at(u, v) < sim)
            {
                sim = cosineMat.at(u, v);
            }
        }
    }
    return sim;
}

void recalculate_similarity_matrix(const NetworkMatrix &cosineMat, NetworkMatrix &clusterMat,
                                   const std::map<uint, Cluster> &clusters,
                                   const Cluster &newCluster, const LinkageType linkage)
{
    // We have to recalculate similarity for each cluster according to newCluster.
    for (const std::pair<uint, Cluster> &c : clusters)
    {
        // We don't recalculate similarity to same cluster.
        if (c.first == newCluster.representative)
            continue;

        float similarity;

        switch (linkage)
        {
        case LinkageType_Single:
            similarity = get_single_linkage_similarity(cosineMat, c.second.vertices, newCluster.vertices);
            break;
        case LinkageType_Complete:
            similarity = get_complete_linkage_similarity(cosineMat, c.second.vertices, newCluster.vertices);
            break;
        case LinkageType_Average:
            similarity = get_average_linkage_similarity(cosineMat, c.second.vertices, newCluster.vertices);
            break;

        default:
            assert(false);
            break;
        }

        clusterMat.at(c.first, newCluster.representative) = similarity;
        clusterMat.at(newCluster.representative, c.first) = similarity;
    }
}

ClusterCandidate find_clusters_to_combine(const NetworkMatrix &clusterMat,
                                          const std::map<uint, Cluster> &clusters)
{
    ClusterCandidate candidate = {};
    candidate.similarity = -999.99f;

    for (const std::pair<uint, Cluster> &cI : clusters)
    {
        for (const std::pair<uint, Cluster> &cJ : clusters)
        {
            if (cI.first == cJ.first || cI.second.isDeleted || cJ.second.isDeleted)
                continue;

            float currSim = clusterMat.at(cI.first, cJ.first);
            if (currSim > candidate.similarity)
            {
                candidate.set_new(cI.second, cJ.second, currSim);
            }
        }
    }

    assert(candidate.similarity != -999.99f);
    return candidate;
}

void delete_cluster(std::map<uint, Cluster> &clusters, const uint key)
{
    size_t removedCount = clusters.erase(key);
    assert(removedCount == 1);
}

size_t get_cluster_count(const std::vector<Cluster> &clusters)
{
    size_t result = 0;
    for (size_t i = 0; i < clusters.size(); i++)
    {
        if (!clusters[i].isDeleted)
            ++result;
    }
    return result;
}

void NetworkMatrix::remove_edges_outside_clusters(const std::vector<Cluster> &clusters)
{
    for (const Cluster &c : clusters)
    {
        for (uint row = 0; row < this->rowCount; row++)
        {
            for (uint col = row + 1; col < this->colCount; col++)
            {
                // If there is edge check if edge is from cluster out.
                if (!is_infinity(row, col) && at(row, col) > 0.0f)
                {
                    // If edge is from cluster outside, remove edge.
                    if (find(c.vertices, row) && !find(c.vertices, col))
                    {
                        at(row, col) = 0.0f;
                    }
                }
            }
        }
    }
}

void NetworkMatrix::hierarchical_clustering(const uint clusterCount, const char *reportFile, LinkageType linkType)
{
    auto clusters = find_clusters_hierarchical(clusterCount, linkType);

    int id = 1;
    for (const Cluster &c : clusters)
    {
        printf("Cluster %i, vertex count: %lu\nVertices: ", id++, c.vertices.size());
        for (const uint &v : c.vertices)
            printf("%u, ", v);

        printf("\n");
    }

    remove_edges_outside_clusters(clusters);
    export_network(reportFile, true);
}

std::vector<Cluster> NetworkMatrix::find_clusters_hierarchical(const uint clusterCount, LinkageType linkType) const
{
    const NetworkMatrix cosineMat = get_cosine_similarity_matrix();
    NetworkMatrix clusterMat = NetworkMatrix(cosineMat);

    std::map<uint, Cluster> clusters;

    // At start every vertex is in cluster of its own.
    for (uint vertex = 0; vertex < rowCount; vertex++)
        clusters[vertex] = Cluster(vertex);

    uint currentClusterCount = clusters.size();

    // Until we have required cluster count...
    while (currentClusterCount > clusterCount)
    {
        // Fint two closest clusters.

        ClusterCandidate candidate = find_clusters_to_combine(clusterMat, clusters);

        // Merge the two closest clusters.
        size_t beforeDelete = clusters.size(); // get_cluster_count(clusters);
        delete_cluster(clusters, candidate.i.representative);
        delete_cluster(clusters, candidate.j.representative);
        size_t afterDelete = clusters.size(); //get_cluster_count(clusters);

        assert(afterDelete == beforeDelete - 2);

        Cluster newCluster(candidate.i.vertices, candidate.j.vertices);

        clusters[newCluster.representative] = newCluster;
        currentClusterCount = clusters.size(); // get_cluster_count(clusters);

        // Recalculate the cosineMat.
        recalculate_similarity_matrix(cosineMat, clusterMat, clusters, newCluster, linkType);
    }
    std::vector<Cluster> result;
    for (const std::pair<uint, Cluster> &c : clusters)
    {
        result.push_back(c.second);
    }
    printf("We are done with hierarchical clustering. Final cluster count: %lu\n", result.size());
    return result;
}
NetworkMatrix::NetworkMatrix(const std::vector<IrisRecord> &vectorData)
{
    size_t size = vectorData.size();
    assert(size > 0);

    this->rowCount = size;
    this->colCount = size;

    this->data.resize(size * size);
    set_matrix_to_one_value(0.0f);

    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = row; col < this->colCount; col++)
        {
            if (row == col)
            {
                at(row, col) = 1.0f;
                at(col, row) = 1.0f;
            }
            else
            {
                float gaussSimilarity = vectorData[row].gauss_distance(vectorData[col]);
                at(row, col) = gaussSimilarity;
                at(col, row) = gaussSimilarity;
            }
        }
    }
    initialize_deleted();
}

void NetworkMatrix::reinitialize(const uint &rowCount, const uint &colCount)
{
    this->rowCount = rowCount;
    this->colCount = colCount;
    this->data.clear();
    this->data.shrink_to_fit();
    this->data.resize(this->rowCount * this->colCount);
    set_matrix_to_one_value(0.0f);
}

NetworkMatrix::~NetworkMatrix()
{
    this->data.clear();
}

uint NetworkMatrix::cols() const
{
    return this->colCount;
}

uint NetworkMatrix::rows() const
{
    return this->rowCount;
}

inline float &NetworkMatrix::at(const uint &row, const uint &col)
{
    return this->data[((row * this->colCount) + col)];
}

inline float &NetworkMatrix::at_vec(const std::vector<float> &vec, const uint &row, const uint &col)
{
    return data[((row * this->colCount) + col)];
}

inline const float &NetworkMatrix::at(const uint &row, const uint &col) const
{
    return this->data[((row * this->colCount) + col)];
}

inline bool NetworkMatrix::is_infinity(const uint &row, const uint &col) const
{
    bool result = at(row, col) == INFINITY;
    return result;
}

uint NetworkMatrix::vertex_count() const
{
    assert(this->rowCount == this->colCount);
    return count(deleted, false);
    //return (this->rowCount - this->deletedVertices.size());
}

uint NetworkMatrix::edge_count() const
{
    uint result = 0;

    for (uint row = 0; row < this->rowCount; row++)
    {
        if (deleted[row])
            continue;
        for (uint col = 0; col < this->colCount; col++)
        {
            if (deleted[row] || deleted[col])
                continue;

            if (!is_infinity(row, col) && (at(row, col) > 0.0f))
            {
                result++;
            }
        }
    }
    return (uint)(result / 2);
}
NetworkMatrix NetworkMatrix::operator+(const NetworkMatrix &other)
{
    assert(this->rowCount == other.rows() && this->colCount == other.cols());

    NetworkMatrix result = NetworkMatrix(this->rowCount, this->colCount);

    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = 0; col < this->colCount; col++)
        {
            result.at(row, col) = at(row, col) - other.at(row, col);
        }
    }

    return result;
}
NetworkMatrix &NetworkMatrix::operator+=(const NetworkMatrix &other)
{
    assert(this->rowCount == other.rows() && this->colCount == other.cols());
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = 0; col < this->colCount; col++)
        {
            at(row, col) += other.at(row, col);
        }
    }
    return *this;
}

NetworkMatrix NetworkMatrix::operator-(const NetworkMatrix &other)
{
    NetworkMatrix result = NetworkMatrix(this->rowCount, this->colCount);

    assert(this->rowCount == other.rows() && this->colCount == other.cols());
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = 0; col < this->colCount; col++)
        {
            result.at(row, col) = at(row, col) - other.at(row, col);
        }
    }

    return result;
}

NetworkMatrix &NetworkMatrix::operator-=(const NetworkMatrix &other)
{
    assert(this->rowCount == other.rows() && this->colCount == other.cols());
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = 0; col < this->colCount; col++)
        {
            at(row, col) -= other.at(row, col);
        }
    }
    return *this;
}

bool NetworkMatrix::equals(const NetworkMatrix &other)
{
    if (this->rowCount != other.rows() || this->colCount != other.cols())
        return false;

    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = 0; col < this->colCount; col++)
        {
            if (at(row, col) != other.at(row, col))
                return false;
        }
    }
    return true;
}

bool NetworkMatrix::operator==(const NetworkMatrix &other)
{
    return equals(other);
}
bool NetworkMatrix::operator!=(const NetworkMatrix &other)
{
    return !equals(other);
}

void NetworkMatrix::copy_data(const NetworkMatrix &source)
{
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = 0; col < this->colCount; col++)
        {
            at(row, col) = source.at(row, col);
        }
    }
}

void NetworkMatrix::insert(const NetworkMatrix &other)
{
    assert(other.rows() <= this->rowCount && other.cols() <= this->colCount);
    for (uint row = 0; row < other.rows(); row++)
    {
        for (uint col = 0; col < other.cols(); col++)
        {
            at(row, col) = other.at(row, col);
        }
    }
}

void NetworkMatrix::set_matrix_to_one_value(const float value)
{
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = row; col < this->colCount; col++)
        {
            at(row, col) = value;
            at(col, row) = value;
        }
    }
}

std::vector<uint> NetworkMatrix::get_degree_of_vertices() const
{
    std::vector<uint> result;
    result.reserve(vertex_count());

    uint vertexDegree = 0;
    for (uint vertexRow = 0; vertexRow < this->rowCount; vertexRow++)
    {
        if (deleted[vertexRow])
            continue;

        vertexDegree = 0;

        for (uint col = 0; col < this->colCount; col++)
        {
            if (!is_infinity(vertexRow, col) && (at(vertexRow, col) > 0.0f))
            {
                vertexDegree++;
            }
        }
        result.push_back(vertexDegree);
    }

    assert(result.size() == vertex_count());

    return result;
}

uint NetworkMatrix::get_vertex_with_max_degree() const
{
    size_t maxDegree = 0;
    uint result = 0;

    for (uint vertex = 0; vertex < this->rowCount; vertex++)
    {
        if (!deleted[vertex])
        {
            uint vertexDegree = 0;
            for (uint col = 0; col < this->colCount; col++)
            {
                if (!is_infinity(vertex, col) && (at(vertex, col) > 0.0f))
                    vertexDegree++;
            }

            if (vertexDegree > maxDegree)
            {
                maxDegree = vertexDegree;
                result = vertex;
            }
        }
    }

    assert(maxDegree != 0);
    return result;
}

float NetworkMatrix::get_average_degree() const
{
    std::vector<uint> degrees = get_degree_of_vertices();
    float result = (float)sum(degrees) / (float)degrees.size();
    return result;
}

void NetworkMatrix::set_inf_where_no_edge()
{
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = 0; col < this->colCount; col++)
        {
            if (at(row, col) == 0.0f)
            {
                at(row, col) = INFINITY;
            }
        }
    }
}

std::vector<float> NetworkMatrix::get_clustering_coeff_for_all_vertices() const
{
    std::vector<uint> vertices;
    uint vertexCount = vertex_count();
    vertices.reserve(vertexCount);

    for (uint vertex = 0; vertex < vertexCount; vertex++)
    {
        vertices.push_back(vertex);
    }

    return get_clustering_coeff_for_vertices(vertices);
}

std::vector<float> NetworkMatrix::get_clustering_coeff_for_vertices(const std::vector<uint> &vertices) const
{

    std::vector<float> result;
    result.reserve(vertices.size());

    std::vector<uint> neighbors;
    float neighboursEdgeCount;
    float nc;
    float clusteringCoeff;
    for (const uint &vertex : vertices)
    {
        neighbors.clear();
        neighbors = get_neighbors(vertex);

        if (neighbors.size() > 1)
        {
            nc = neighbors.size();
            neighboursEdgeCount = (float)get_edge_count_between_neighbours(neighbors);
            clusteringCoeff = (2.0f * neighboursEdgeCount) / (nc * (nc - 1.0f));

            result.push_back(clusteringCoeff);
        }
        else
        {
            result.push_back(0);
        }
    }

    return result;
}

std::vector<uint> NetworkMatrix::get_neighbors_except(const uint vertex, const std::vector<uint> &except) const
{
    std::vector<uint> result;

    for (uint vertexNeighbourCol = 0; vertexNeighbourCol < this->colCount; vertexNeighbourCol++)
    {
        if (vertex == vertexNeighbourCol || find(except, vertexNeighbourCol))
        {
            continue;
        }
        if (!is_infinity(vertex, vertexNeighbourCol) && at(vertex, vertexNeighbourCol) > 0.0)
            result.push_back(vertexNeighbourCol);
    }

    return result;
}

std::vector<uint> NetworkMatrix::get_neighbors(const uint vertex) const
{
    std::vector<uint> result;

    for (uint vertexNeighbourCol = 0; vertexNeighbourCol < this->colCount; vertexNeighbourCol++)
    {
        if (vertex == vertexNeighbourCol)
            continue;
        if (!is_infinity(vertex, vertexNeighbourCol) && at(vertex, vertexNeighbourCol) > 0.0)
            result.push_back(vertexNeighbourCol);
    }

    return result;
}

uint NetworkMatrix::get_edge_count_between_neighbours(const std::vector<uint> &neighbors) const
{
    uint result = 0;
    for (const uint &n1Index : neighbors)
    {
        for (const uint &n2Index : neighbors)
        {
            if (n1Index == n2Index)
                continue;
            if (!is_infinity(n1Index, n2Index) && at(n1Index, n2Index) > 0.0)
                result++;
        }
    }
    return (result / 2);
}

float NetworkMatrix::get_average_clustering_coefficient() const
{
    std::vector<float> clusteringCoefficients = get_clustering_coeff_for_all_vertices();
    float result = sum(clusteringCoefficients) / (float)clusteringCoefficients.size();
    return result;
}

std::map<uint, std::vector<uint>> NetworkMatrix::get_vertices_grouped_by_degree() const
{
    std::map<uint, std::vector<uint>> result;
    std::vector<uint> degrees = get_degree_of_vertices();

    uint degree;
    for (uint vertex = 0; vertex < degrees.size(); vertex++)
    {
        degree = degrees[vertex];

        if (!contains_key(result, degree))
        {
            result[degree] = std::vector<uint>();
        }
        result[degree].push_back(vertex);
    }

    return result;
}
/************************************************************************************************************/
inline uint get_best_unvisited(const std::vector<uint> &unvisited, const std::vector<float> &distances)
{
    uint best = 0;
    float min = INFINITY;
    for (const uint &vertex : unvisited)
    {
        if (distances[vertex] < min)
        {
            min = distances[vertex];
            best = vertex;
        }
    }
    return best;
}

inline void new_best_distance(const uint &currentVertex, std::vector<uint> &unvisited, std::vector<float> &distances, const float distance)
{
    distances.at(currentVertex) = distance;
    if (!find(unvisited, currentVertex))
    {
        unvisited.push_back(currentVertex);
    }
}

bool NetworkMatrix::can_use_bfs() const
{
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = 0; col < this->colCount; col++)
        {
            if (at(row, col) > 1.0f)
                return false;
        }
    }
    return true;
}

float NetworkMatrix::bfs_path(const NetworkMatrix &mat, const uint &source, const uint &dest) const
{
    std::vector<bool> visited;
    std::vector<int> previous;
    visited.reserve(this->colCount);
    previous.reserve(this->colCount);

    for (uint i = 0; i < this->colCount; i++)
    {
        previous.push_back(-1);
        visited.push_back(false);
    }

    uint current;
    std::queue<uint> q;
    q.push(source);
    previous[source] = -1;

    std::vector<uint> neighbors;
    bool found = false;
    while (!q.empty())
    {
        //printf("%lu in queue\n", size);

        current = q.front();
        q.pop();

        if (current == dest)
        {
            found = true;
            break;
        }

        neighbors = get_neighbors(current);
        for (const uint &neighbor : neighbors)
        {
            if (!visited[neighbor])
            {
                q.push(neighbor);
                visited[neighbor] = true;
                previous[neighbor] = current;
            }
        }
    }
    if (!found)
    {
        return INFINITY;
    }

    float result = 0;
    current = dest;
    while (current != source)
    {
        result++;
        current = previous[current];
    }
    return result;
}

std::vector<uint> NetworkMatrix::find_k_neighbors(const uint row, const uint k) const
{
    std::vector<std::pair<float, uint>> rowValues;
    rowValues.resize(colCount);

    for (uint col = 0; col < this->colCount; col++)
    {
        if (row == col)
            continue;
        rowValues[col] = std::make_pair(at(row, col), col);
    }

    std::sort(rowValues.begin(), rowValues.end());
    uint take = rowValues.size() > k ? k : rowValues.size();

    std::vector<uint> result;
    result.resize(take);
    size_t from = rowValues.size() - 1;
    for (size_t i = 0; i < take; i++)
    {
        result[i] = rowValues[from - i].second;
    }

    return result;
}

uint NetworkMatrix::count_in_e_radius(const uint row, const float e) const
{
    uint result = 0;
    for (uint col = 0; col < this->colCount; col++)
    {
        if (row == col)
            continue;
        if (at(row, col) >= e)
            result++;
    }
    return result;
}

void NetworkMatrix::filter_knn_row(const uint row, const uint k)
{
    auto kNeighbors = find_k_neighbors(row, k);

    for (uint col = 0; col < this->colCount; col++)
    {
        at(row, col) = 0.0f;
    }
    for (const uint &neigh : kNeighbors)
        at(row, neigh) = 1.0f;
}

float NetworkMatrix::dijkstra_path(const NetworkMatrix &mat, const uint &source, const uint &dest) const
{
    uint vc = vertex_count();
    std::vector<uint> unvisited;
    std::vector<float> distances;
    std::vector<bool> visited;
    unvisited.reserve(vc);
    distances.reserve(vc);
    visited.reserve(vc);

    for (uint v = 0; v < vc; v++)
    {
        distances.push_back(INFINITY);
        visited.push_back(false);
    }
    uint current = source;
    new_best_distance(current, unvisited, distances, 0);
    auto neis = get_neighbors(current);
    assert(neis.size() > 0);
    for (const uint &neighbor : neis)
    {
        new_best_distance(neighbor, unvisited, distances, INFINITY);
    }

    while (!visited[dest])
    {
        current = get_best_unvisited(unvisited, distances);
        auto neighbors = get_neighbors(current);
        if (neighbors.size() > 0)
        {
            for (const uint &neighbor : neighbors)
            {
                if (visited[neighbor])
                    continue;
                float distanceToNeighbour = distances[current] + mat.at(current, neighbor);
                if (distanceToNeighbour < distances[neighbor])
                {
                    new_best_distance(neighbor, unvisited, distances, distanceToNeighbour);
                }
            }
        }

        visited[current] = true;
        unvisited.erase(std::remove(std::begin(unvisited), std::end(unvisited), current));
    }

    float result = distances[dest];
    return result;
}

std::vector<float> NetworkMatrix::get_closeness_centrality_for_vertices(const NetworkMatrix &distanceMatrix) const
{
    assert(this->rowCount == this->colCount);

    uint vc = vertex_count();
    std::vector<float> result;
    result.reserve(vc);

    float cc, distanceSum;
    for (uint vertex = 0; vertex < vc; vertex++)
    {
        cc = 1.0f;
        distanceSum = 0.0f;

        for (uint col = 0; col < vc; col++)
        {
            distanceSum += distanceMatrix.at(vertex, col);
        }
        cc = (1.0f / distanceSum) * ((float)vc - 1.0f);
        result.push_back(cc);
    }

    return result;
}
// for rId in range(0, self.rowCount):
//             result.append(round(1 / (sum([self.values[rId][cId] for cId in range(0, self.colCount) if cId != rId]) / vertexCount), 7))

NetworkMatrix NetworkMatrix::get_distance_matrix(const bool forceDijkstra) const
{
    assert(this->rowCount == this->colCount);
    bool bfs = !forceDijkstra && can_use_bfs();
    //printf("Will use %s alg.\n", bfs ? "BreadthFirstSearch" : "Dijkstra");

    NetworkMatrix distanceMat(this->rowCount, this->colCount);
    NetworkMatrix result(this->rowCount, this->colCount);

    distanceMat.copy_data(*this);
    result.copy_data(*this);
    distanceMat.set_inf_where_no_edge();

#pragma omp parallel for
    for (uint row = 0; row < this->rowCount; row++)
    {
        if (deleted[row])
            continue;

        for (uint col = row + 1; col < this->colCount; col++)
        {
            if (deleted[col])
                continue;

            float distance = bfs ? bfs_path(distanceMat, row, col) : dijkstra_path(distanceMat, row, col);
            result.at(row, col) = distance;
            result.at(col, row) = distance;
        }
    }
    return result;
}

void NetworkMatrix::print_matrix() const
{
    std::string result = "";

    for (uint row = 0; row < this->rowCount; row++)
    {
        result += "\t";
        for (uint col = 0; col < this->colCount; col++)
        {
            if (col == (this->colCount - 1))
                result += std::to_string(at(row, col));
            else
                result += std::to_string(at(row, col)) + " ";
        }
        result += "\n";
    }
    printf("%s", result.c_str());
}

float NetworkMatrix::get_probability_for_symmetric_network(const uint vertexCount) const
{
    float result = (float)(log(vertexCount) / (double)vertexCount);
    return result;
}

void NetworkMatrix::generate_random_network(const float edgeProbability, bool autoProbability)
{
    float prob = edgeProbability;
    if (autoProbability)
    {
        prob = get_probability_for_symmetric_network(vertex_count());
        printf("Used probability: %f\n", prob);
    }

    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());

    float weights[] = {(1.0f - prob), prob};
    std::discrete_distribution<int> discreteDistribution = std::discrete_distribution<int>(std::begin(weights), std::end(weights));

    int edge;
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = row; col < this->colCount; col++)
        {
            if (row == col)
            {
                at(row, col) = 0;
            }
            else
            {
                edge = discreteDistribution(randomGenerator);
                at(row, col) = edge;
                at(col, row) = edge;
            }
        }
    }

    printf("Random: VC: %u; EC: %u\n", vertex_count(), edge_count());
}

void NetworkMatrix::generate_scale_free_network(uint numberOfConnections, const uint numberOfVerticesToAdd)
{
    generate_scale_free_network(3, 3 + numberOfVerticesToAdd, numberOfConnections);
}

void NetworkMatrix::generate_scale_free_network(const uint initialSize, const uint finalSize, const uint numberOfConnections)
{
    uint resultSize = finalSize;
    uint currentSize = initialSize;
    reinitialize(resultSize, resultSize);
    std::vector<uint> vertexList;
    if (initialSize == 3)
    {
        NetworkMatrix initialMat3 = get_initial_matrix_of_size_3();
        vertexList = {0, 0, 1, 1, 2, 2};
        insert(initialMat3);
    }
    else
    {
        NetworkMatrix initial = NetworkMatrix(initialSize, initialSize);
        for (uint v = 0; v < initialSize; v++)
        {
            if (v != (initialSize - 1))
            {
                initial.at(v, v + 1) = 1.0f;
                initial.at(v + 1, v) = 1.0f;
            }

            vertexList.push_back(v);
            if (v != 0 && v != (initialSize - 1))
            {
                vertexList.push_back(v);
            }
        }
        insert(initial);
    }

    uint newVertexIndex, neighbor;
    float vertexListSize;

    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());
    std::discrete_distribution<int> discreteDistribution;

    std::vector<float> weights;
    std::vector<uint> neighbors;

    for (uint step = 0; step < (finalSize - initialSize); step++)
    {
        weights.clear();
        neighbors.clear();

        weights.reserve(currentSize);
        neighbors.reserve(numberOfConnections);

        newVertexIndex = initialSize + step;
        vertexListSize = (float)vertexList.size();

        float ageScaling = 1.0f;
        for (uint vertex = 0; vertex < currentSize; vertex++)
        {
            float age = (initialSize + step) - vertex;
            float occurence = (float)count(vertexList, vertex);
            float weight = (occurence / vertexListSize);

            float agedWeight = weight * pow(age, (-1.0f * ageScaling));

            //weights.push_back(weight);
            weights.push_back(agedWeight);
        }

        discreteDistribution = std::discrete_distribution<int>(std::begin(weights), std::end(weights));
        for (uint neighbourStep = 0; neighbourStep < numberOfConnections; neighbourStep++)
        {

            neighbor = discreteDistribution(randomGenerator);
            while (find(neighbors, neighbor))
            {
                neighbor = discreteDistribution(randomGenerator);
            }
            neighbors.push_back(neighbor);
        }

        assert(neighbors.size() == numberOfConnections);

        vertexList.push_back(newVertexIndex);
        vertexList.push_back(newVertexIndex);

        for (const uint &newNeighbour : neighbors)
        {
            vertexList.push_back(newNeighbour);
            at(newVertexIndex, newNeighbour) = 1.0;
            at(newNeighbour, newVertexIndex) = 1.0;
        }

        currentSize++;
    }

    printf("ScaleFree: VC: %u; EC: %u\n", vertex_count(), edge_count());
}

void NetworkMatrix::generate_holme_kim(float probability, const uint newVertexConnectionsCount)
{
    uint startingSize = 3;
    uint resultSize = vertex_count();
    uint currentSize = startingSize;

    NetworkMatrix initialMat = get_initial_matrix_of_size_3();

    std::vector<uint> vertexList = {0, 0, 1, 1, 2, 2};

    reinitialize(resultSize, resultSize);
    insert(initialMat);

    uint newVertexIndex, neighbor;
    float vertexListSize;

    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());
    std::discrete_distribution<int> discreteDistribution;

    std::discrete_distribution<int> chooseNeighborOfNeighbor({1 - probability, probability});

    std::vector<float> weights;
    std::vector<uint> neighbors;
    for (uint step = 0; step < (resultSize - startingSize); step++)
    {
        weights.clear();
        neighbors.clear();

        weights.reserve(currentSize);
        neighbors.reserve(newVertexConnectionsCount);

        newVertexIndex = startingSize + step;
        vertexListSize = (float)vertexList.size();

        for (uint vertex = 0; vertex < currentSize; vertex++)
        {
            weights.push_back(((float)count(vertexList, vertex) / vertexListSize));
        }

        discreteDistribution = std::discrete_distribution<int>(std::begin(weights), std::end(weights));
        bool doPA = false;
        uint lastConnectedVertex = 0;
        for (uint neighbourStep = 0; neighbourStep < newVertexConnectionsCount; neighbourStep++)
        {
            if (neighbourStep == 0) // In first step always use PA.
                doPA = true;
            else
            {
                int pa = chooseNeighborOfNeighbor(randomGenerator);
                if (pa) // Choose some random neighbot of neighbor
                    doPA = false;
                else // Do another pa step.
                {
                    doPA = true;
                }
            }

            if (doPA)
            {
                do
                {
                    neighbor = discreteDistribution(randomGenerator);
                } while (find(neighbors, neighbor));
                neighbors.push_back(neighbor);
                lastConnectedVertex = neighbor;
            }
            else
            {
                auto lastConnectedNeighbors = get_neighbors(lastConnectedVertex);
                std::uniform_int_distribution<> randNeigh(0, lastConnectedNeighbors.size() - 1);
                do
                {
                    neighbor = lastConnectedNeighbors[randNeigh(randomGenerator)];
                } while (find(neighbors, neighbor));

                neighbors.push_back(neighbor);
                lastConnectedVertex = neighbor;
            }
        }

        assert(neighbors.size() == newVertexConnectionsCount);

        vertexList.push_back(newVertexIndex);
        vertexList.push_back(newVertexIndex);

        for (const uint &newNeighbour : neighbors)
        {
            vertexList.push_back(newNeighbour);
            at(newVertexIndex, newNeighbour) = 1.0;
            at(newNeighbour, newVertexIndex) = 1.0;
        }

        currentSize++;
    }

    printf("VC: %u; EC: %u\n", vertex_count(), edge_count());
}

void NetworkMatrix::generate_bianconi(float probability, const uint newVertexConnectionsCount)
{
    uint startingSize = 3;
    uint resultSize = vertex_count();
    uint currentSize = startingSize;

    NetworkMatrix initialMat = get_initial_matrix_of_size_3();

    reinitialize(resultSize, resultSize);
    insert(initialMat);

    uint newVertexIndex, neighbor;

    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());

    std::discrete_distribution<int> chooseNeighborOfNeighbor({1 - probability, probability});
    std::vector<uint> neighbors;
    std::uniform_int_distribution<> randomVertexGenerator;
    for (uint step = 0; step < (resultSize - startingSize); step++)
    {
        neighbors.clear();
        neighbors.reserve(newVertexConnectionsCount);

        newVertexIndex = startingSize + step;
        randomVertexGenerator = std::uniform_int_distribution<>(0, newVertexIndex - 1);
        uint newRandomNeigh = randomVertexGenerator(randomGenerator);

        neighbors.push_back(newRandomNeigh);
        uint lastConnectedVertex = newRandomNeigh;

        for (uint neighbourStep = 0; neighbourStep < newVertexConnectionsCount - 1; neighbourStep++)
        {
            if (chooseNeighborOfNeighbor(randomGenerator)) // Choose some random neighbor of neighbor
            {
                auto lastConnectedNeighbors = get_neighbors(lastConnectedVertex);
                std::uniform_int_distribution<> randNeigh(0, lastConnectedNeighbors.size() - 1);
                do
                {
                    neighbor = lastConnectedNeighbors[randNeigh(randomGenerator)];
                } while (find(neighbors, neighbor));

                neighbors.push_back(neighbor);
                lastConnectedVertex = neighbor;
            }
            else
            {
                do
                {
                    neighbor = randomVertexGenerator(randomGenerator);
                } while (find(neighbors, neighbor));

                neighbors.push_back(neighbor);
                lastConnectedVertex = neighbor;
            }
        }

        assert(neighbors.size() == newVertexConnectionsCount);

        for (const uint &newNeighbour : neighbors)
        {
            at(newVertexIndex, newNeighbour) = 1.0;
            at(newNeighbour, newVertexIndex) = 1.0;
        }

        currentSize++;
    }

    printf("VC: %u; EC: %u\n", vertex_count(), edge_count());
}

std::vector<std::pair<uint, uint>> NetworkMatrix::get_edges() const
{
    std::vector<std::pair<uint, uint>> edges;

    for (uint row = 0; row < this->rowCount; row++)
    {
        if (deleted[row])
            continue;
        for (uint col = row + 1; col < this->colCount; col++)
        {
            if (deleted[col])
                continue;

            if (!is_infinity(row, col) && at(row, col) > 0.0f)
                edges.push_back(std::make_pair(row, col));
        }
    }

    return edges;
}

void NetworkMatrix::generate_select_link_or_copy(bool copy, float copyProbability)
{
    uint startingSize = 3;
    uint resultSize = vertex_count();

    NetworkMatrix initialMat = get_initial_matrix_of_size_3();
    reinitialize(resultSize, resultSize);
    insert(initialMat);

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> neighRandom;
    std::uniform_int_distribution<int> linkEndRandom(0, 1);
    std::discrete_distribution<int> copyRandom({1 - copyProbability, copyProbability});

    std::vector<std::pair<uint, uint>> edges = get_edges();

    std::pair<uint, uint> edge;
    uint newVertexIndex;
    for (uint step = 0; step < (resultSize - startingSize); step++)
    {
        newVertexIndex = startingSize + step;
        std::uniform_int_distribution<int> edgeRandom(0, (int)(edges.size() - 1));
        edge = edges[edgeRandom(rd)];

        uint partner;
        if (copy)
        {
            // Copy wth probability select edge.first with (1-probability) select edge.first random neigbor.
            if (copyRandom(rd))
            {
                // Select edge.first
                partner = edge.first;
            }
            else
            {
                // Select random neighbor of edge.first
                auto neighbors = get_neighbors(edge.first);
                neighRandom = std::uniform_int_distribution<int>(0, (int)(neighbors.size() - 1));
                partner = neighbors[neighRandom(rd)];
            }
        }
        else
        {
            // Link selection, select one of vertices on the edge.
            partner = linkEndRandom(rd) ? edge.first : edge.second;
        }

        at(newVertexIndex, partner) = 1.0f;
        at(partner, newVertexIndex) = 1.0f;
        edges.push_back(std::make_pair(newVertexIndex, partner));
    }
    printf("VC: %u; EC: %u\n", vertex_count(), edge_count());
}

void NetworkMatrix::load_from_edges(const std::vector<std::pair<uint, uint>> &edges, int offset)
{
    uint rowIndex, colIndex;
    for (const std::pair<uint, uint> &edge : edges)
    {
        rowIndex = edge.first + offset;
        colIndex = edge.second + offset;
        // if (rowIndex >= rowCount || colIndex >= colCount)
        // {
        //     printf("wha th  fucnk\n");
        // }
        at(rowIndex, colIndex) = 1.0f;
        at(colIndex, rowIndex) = 1.0f;
    }
}

void NetworkMatrix::export_network(const char *filename, bool allSelfEdge) const
{
    std::vector<std::pair<uint, uint>> edges;
    for (uint row = 0; row < this->rowCount; row++)
    {
        if (allSelfEdge)
            edges.push_back(std::make_pair(row, row));

        for (uint col = row; col < this->colCount; col++)
        {
            if (!is_infinity(row, col) && (at(row, col) > 0.0f))
            {
                edges.push_back(std::make_pair(row, col));
            }
        }
    }
    save_network(filename, edges);
}

float NetworkMatrix::get_network_longest_distance(const NetworkMatrix &distanceMatrix) const
{
    float max = 0;
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = row + 1; col < this->colCount; col++)
        {
            if (distanceMatrix.at(row, col) > max)
                max = distanceMatrix.at(row, col);
        }
    }
    return max;
}

float NetworkMatrix::get_network_average_distance(const NetworkMatrix &distanceMatrix) const
{
    assert(this->rowCount == this->colCount);
    float distanceSum = 0;
    for (uint row = 0; row < this->rowCount; row++)
    {
        if (deleted[row])
            continue;

        for (uint col = row + 1; col < this->colCount; col++)
        {
            if (deleted[col])
                continue;
            if (distanceMatrix.at(row, col) != INFINITY)
            {
                distanceSum += distanceMatrix.at(row, col);
            }
        }
    }

    uint vertexCount = vertex_count();

    float result = distanceSum / (float)((vertexCount * (vertexCount - 1)) / 2.0f);
    return result;
}
std::vector<float> NetworkMatrix::get_eccentricities(const NetworkMatrix &distanceMatrix) const
{
    assert(this->rowCount == this->colCount);
    std::vector<float> result;
    result.reserve(this->rowCount);

    float max;
    for (uint vertex = 0; vertex < this->rowCount; vertex++)
    {
        max = 0;

        for (uint col = 0; col < this->colCount; col++)
        {
            if (distanceMatrix.at(vertex, col) > max)
                max = distanceMatrix.at(vertex, col);
        }
        result.push_back(max);
    }

    return result;
}

void NetworkMatrix::complete_analysis(const char *networkName, const char *filename, const bool verbose, const bool complete) const
{
    using namespace std;

    std::ofstream outStream(filename);
    assert(outStream.is_open() && "Failed to open report file.");

    outStream << "=====================Analysis for network: " << networkName << "=====================" << endl;

    uint vertexCount = vertex_count();

    if (verbose)
        printf("Vertex count: %i\n", vertexCount);
    outStream << "Vertex count: " << vertexCount << endl;

    uint edgeCount = edge_count();
    if (verbose)
        printf("Edge count: %i\n", edgeCount);
    outStream << "Edge count: " << edgeCount << endl;

    // Vertex degree distribution
    std::vector<uint> degrees = get_degree_of_vertices();
    outStream << "Degree distribution:" << endl;

    for (size_t i = 0; i < degrees.size(); i++)
        outStream << degrees[i] << ";";
    outStream << endl
              << endl;

    // Average vertex degree.
    float averageDegree = get_average_degree();
    if (verbose)
        printf("Average degree: %f\n", averageDegree);
    outStream << "Average degree: " << averageDegree << endl;

    // Clustering coefficient for vertices.
    std::vector<float> clusteringCofficients = get_clustering_coeff_for_all_vertices();
    outStream << "Clustering coefficients:" << endl;

    for (size_t i = 0; i < clusteringCofficients.size(); i++)
        outStream << clusteringCofficients[i] << ";";
    outStream << endl
              << endl;

    // Average clustering coefficient.
    float averageClusteringCoefficient = get_average_clustering_coefficient();
    if (verbose)
        printf("Average clustering coefficient: %f\n", averageClusteringCoefficient);
    outStream << "Average clustering coefficient: " << averageClusteringCoefficient << endl;

    if (complete)
    {
        // Closeness centrality.
        NetworkMatrix distanceMatrix = get_distance_matrix();
        std::vector<float> closenessCentrality = get_closeness_centrality_for_vertices(distanceMatrix);
        outStream << "Closeness centralities:" << endl;

        for (size_t i = 0; i < closenessCentrality.size(); i++)
        {
            outStream << closenessCentrality[i] << ";";
            if (verbose)
                printf("%f;", closenessCentrality[i]);
        }
        outStream << endl
                  << endl;
        if (verbose)
            printf("\n");

        // Network average. Longest distance.
        float networkAverage = get_network_longest_distance(distanceMatrix);
        if (verbose)
            printf("Network average: %f\n", networkAverage);
        outStream << "Network average: " << networkAverage << endl;

        // Network average distance.
        float averageDistance = get_network_average_distance(distanceMatrix);
        if (verbose)
            printf("Average distance: %f\n", averageDistance);
        outStream << "Average distance: " << averageDistance << endl;

        // Vertices eccentricity.
        std::vector<float> eccentricities = get_eccentricities(distanceMatrix);
        outStream << "Eccentricities:" << endl;

        for (size_t i = 0; i < eccentricities.size(); i++)
            outStream << eccentricities[i] << ";";
        outStream << endl
                  << endl;
    }

    outStream << "======================================END==================================" << endl;
    outStream.flush();
    outStream.close();
}

void NetworkMatrix::filter_e_radius(const float radius)
{
    for (size_t i = 0; i < this->data.size(); i++)
    {
        if (data[i] >= radius)
        {
            data[i] = 1.0f;
        }
        else
        {
            data[i] = 0.0f;
        }
    }
}

void NetworkMatrix::filter_kNN(const uint k)
{
    for (uint row = 0; row < this->rowCount; row++)
    {
        //filter_knn_row(row, k);
        auto kNeighbors = find_k_neighbors(row, k);

        for (uint col = 0; col < this->colCount; col++)
        {
            at(row, col) = 0.0f;
        }
        for (const uint &neigh : kNeighbors)
            at(row, neigh) = 1.0f;
    }
}

void NetworkMatrix::filter_combinataion_e_knn(const float radius, const uint k)
{
    uint inRadiusCount = 0;
    for (uint row = 0; row < this->rowCount; row++)
    {
        inRadiusCount = count_in_e_radius(row, radius);
        if (inRadiusCount > k)
        {
            //Take in radius.
            for (uint col = 0; col < this->colCount; col++)
            {
                if (at(row, col) >= radius)
                    at(row, col) = 1.0f;
                else
                    at(row, col) = 0.0f;
            }
        }
        else
        {
            // Take K neighbors.
            filter_knn_row(row, k);
        }
    }
}

NetworkMatrix NetworkMatrix::filter_random_node_sampling(const float targetPercentSize) const
{
    printf("Random based sampling\n");
    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());

    float weights[] = {(1.0f - targetPercentSize), targetPercentSize};
    std::discrete_distribution<int> discreteDistribution = std::discrete_distribution<int>(std::begin(weights), std::end(weights));

    bool isInSample;
    std::vector<uint> sample;
    sample.reserve(this->rowCount);

    uint threshold = (uint)((float)rowCount * targetPercentSize);

    for (size_t i = 0; i < this->rowCount; i++)
    {
        isInSample = discreteDistribution(randomGenerator) == 1;
        if (isInSample)
        {
            sample.push_back(i);
            for (const uint n : get_neighbors(i))
            {
                if (!find(sample, n))
                    sample.push_back(n);
            }
        }
        if (sample.size() > threshold)
            break;
    }
    size_t sampleSize = sample.size();
    NetworkMatrix sampleNetwork = NetworkMatrix(sampleSize, sampleSize);

    uint u, v;
    for (uint sampleRow = 0; sampleRow < sampleSize; sampleRow++)
    {
        u = sample[sampleRow];
        for (uint sampleCol = sampleRow + 1; sampleCol < sampleSize; sampleCol++)
        {
            v = sample[sampleCol];
            if (!is_infinity(u, v) && at(u, v) > 0.0f)
            {
                sampleNetwork.at(sampleRow, sampleCol) = 1.0f;
            }
        }
    }

    printf("Random Node sampling results: VC: %5u EC: %5u\n", sampleNetwork.vertex_count(), sampleNetwork.edge_count());
    return sampleNetwork;
}

NetworkMatrix NetworkMatrix::filter_random_edge_sampling(const float targetPercentSize) const
{
    printf("Degree based sampling\n");
    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());
    std::discrete_distribution<int> discreteDistribution;

    std::vector<uint> sample;
    sample.reserve(rowCount);

    auto degrees = get_degree_of_vertices();

    uint threshold = (uint)((float)rowCount * targetPercentSize);
    float weights[2];
    float prob;
    for (size_t row = 0; row < rowCount; row++)
    {
        prob = targetPercentSize / (float)degrees[row];
        weights[0] = 1.0f - prob;
        weights[1] = prob;
        discreteDistribution = std::discrete_distribution<int>(std::begin(weights), std::end(weights));
        if (discreteDistribution(randomGenerator) == 1)
        {
            //sample.push_back(row);
            sample.push_back(row);
            for (const uint n : get_neighbors(row))
            {
                if (!find(sample, n))
                    sample.push_back(n);
            }

            if (sample.size() > threshold)
                break;
        }
    }
    size_t sampleSize = sample.size();
    NetworkMatrix sampleNetwork(sampleSize, sampleSize);

    uint u, v;
    for (uint sampleRow = 0; sampleRow < sampleSize; sampleRow++)
    {
        u = sample[sampleRow];
        for (uint sampleCol = sampleRow + 1; sampleCol < sampleSize; sampleCol++)
        {
            v = sample[sampleCol];
            if (!is_infinity(u, v) && at(u, v) > 0.0f)
            {
                sampleNetwork.at(sampleRow, sampleCol) = 1.0f;
            }
        }
    }

    printf("Random Edge Sampling results: VC: %5u EC: %5u\n", sampleNetwork.vertex_count(), sampleNetwork.edge_count());
    return sampleNetwork;
}

uint NetworkMatrix::get_edge_count_between_groups(const std::vector<uint> &gA, const std::vector<uint> &gB) const
{
    uint result = 0;
    for (auto &&a : gA)
    {
        for (auto &&b : gB)
        {
            assert(a != b);
            assert(!is_infinity(a, b));
            result += at(a, b);
        }
    }
    return result;
}

struct SwapInfo
{
    uint aIndex;
    uint bIndex;
    uint cut;

    SwapInfo(uint _a, uint _b, uint _cut) : aIndex(_a), bIndex(_b), cut(_cut)
    {
    }

    bool operator>(const SwapInfo &other) { return this->cut > other.cut; }
    bool operator>=(const SwapInfo &other) { return this->cut >= other.cut; }
    bool operator<(const SwapInfo &other) { return this->cut < other.cut; }
    bool operator<=(const SwapInfo &other) { return this->cut <= other.cut; }
};

void NetworkMatrix::kernighan_lin() const
{
    const size_t groupCount = 2;
    int groupSize = (this->rowCount + groupCount - 1) / 2;
    std::vector<uint> alreadySwapped;
    alreadySwapped.reserve(this->rowCount);

    std::vector<uint> groupA;
    std::vector<uint> groupB;
    groupA.reserve(groupSize);
    groupB.reserve(groupSize);

    std::vector<uint> indices;
    indices.resize(rowCount);
    for (size_t i = 0; i < this->rowCount; i++)
        indices[i] = i;

    std::random_shuffle(indices.begin(), indices.end());

    for (uint i = 0; i < this->rowCount; i++)
    {
        if (i % 2 == 0)
            groupA.push_back(indices[i]);
        else
            groupB.push_back(indices[i]);
    }

    uint lastCut = get_edge_count_between_groups(groupA, groupB);
    uint cut = lastCut;
    int iter = 0;

    do
    {
        // one step.
        std::vector<SwapInfo> stepSwaps;
        for (size_t a = 0; a < groupA.size(); a++)
        {
            for (size_t b = 0; b < groupB.size(); b++)
            {
                uint vA = groupA[a];
                uint vB = groupB[b];

                if (find(alreadySwapped, vA) || find(alreadySwapped, vB))
                    continue;

                groupA[a] = vB;
                groupB[b] = vA;

                uint cut = get_edge_count_between_groups(groupA, groupB);
                stepSwaps.push_back(SwapInfo(a, b, cut));

                groupA[a] = vA;
                groupB[b] = vB;
            }
        }

        std::sort(stepSwaps.begin(), stepSwaps.end());
        SwapInfo bestSwap = stepSwaps[0];
        // Swap them.
        uint vA = groupA[bestSwap.aIndex];
        uint vB = groupB[bestSwap.bIndex];
        groupA[bestSwap.aIndex] = vB;
        groupB[bestSwap.bIndex] = vA;
        alreadySwapped.push_back(vA);
        alreadySwapped.push_back(vB);

        lastCut = cut;
        cut = get_edge_count_between_groups(groupA, groupB);

        if (cut > lastCut)
        {
            groupA[bestSwap.aIndex] = vA;
            groupB[bestSwap.bIndex] = vB;
            cut = get_edge_count_between_groups(groupA, groupB);
            printf("Correcting result. Swapping pair back. Final cut: %4u\n", cut);
            break;
        }

        printf("Finished iterataion %i; Went from %4u to %4u.\n", ++iter, lastCut, cut);
    } while (cut < lastCut);

    printf("First group: ");
    print_vector(groupA);
    printf("Second group: ");
    print_vector(groupB);
}

void NetworkMatrix::delete_edges_for(const uint vertex)
{
    for (uint col = 0; col < this->colCount; col++)
    {
        at(vertex, col) = 0.0f;
        at(col, vertex) = 0.0f;
    }
}

void NetworkMatrix::filter_k_core(const uint k)
{
    //NetworkMatrix editMat = NetworkMatrix(*this);

    /*
    Postup – odstraníme všechny vrcholy se stupněm < k (nemohou být součástí k-core), 
        toto opakujeme tak dlouho, dokud se v grafu vyskytují vrcholy se stupněm <k. 
        Nakonec zůstane množina všech k-core
    */

    std::vector<uint> kCoreVertices;
    kCoreVertices.resize(rowCount);
    for (size_t i = 0; i < rowCount; i++)
        kCoreVertices[i] = i;

    bool reduce = true;
    while (reduce)
    {
        reduce = false;
        std::vector<uint> tmpKCoreVertices;
        std::vector<uint> degrees = get_degree_of_vertices();

        for (const uint &v : kCoreVertices)
        {
            if (degrees[v] >= k)
            {
                tmpKCoreVertices.push_back(v);
            }
            else
            {
                delete_edges_for(v);
                reduce = true;
            }
        }

        kCoreVertices.swap(tmpKCoreVertices);
    }
    printf("Final vertex count %lu\n", kCoreVertices.size());
}

std::vector<GraphComponent> NetworkMatrix::get_components() const
{
    std::vector<GraphComponent> result;

    uint componentId = 0;
    std::vector<uint> notProcessed;
    std::vector<uint> processed;
    notProcessed.reserve(this->rowCount);
    processed.reserve(this->rowCount);

    for (size_t i = 0; i < this->rowCount; i++)
        notProcessed[i] = i;

    for (uint v = 0; v < this->rowCount; v++)
    {
        if (!find(processed, v))
        {
            processed.push_back(v);
            GraphComponent component(componentId++, v);

            std::vector<uint> lastNeighborhood = get_neighbors_except(v, processed);
            push_range(component.vertices, lastNeighborhood);
            push_range(processed, lastNeighborhood);

            bool foundNew = true;
            while (foundNew)
            {
                foundNew = false;

                std::vector<uint> newNeighborhood;
                for (const uint &neigh : lastNeighborhood)
                {
                    auto newNeighs = get_neighbors_except(neigh, processed);
                    if (newNeighs.size() > 0)
                    {
                        foundNew = true;
                        push_range(component.vertices, newNeighs);
                        push_range(processed, newNeighs);
                        push_range(newNeighborhood, newNeighs);
                    }
                }
                lastNeighborhood = newNeighborhood;
            }

            result.push_back(component);
        }
    }

    uint componentVertexCount = 0;
    for (const GraphComponent &comp : result)
        componentVertexCount += comp.vertices.size();

    assert(componentVertexCount == this->rowCount);
    assert(processed.size() == this->rowCount);

    return result;
}

void NetworkMatrix::print_network_stats(const char *header) const
{
    auto components = get_components();
    auto maxComponent = get_biggest_component(components);
    float avgDistance = get_network_average_distance(get_distance_matrix());
    float avgDegree = get_average_degree();

    printf("%s\n", header);
    printf("%-25s: %8u\n", "VC", vertex_count());
    printf("%-25s: %8u\n", "EC", edge_count());
    printf("%-25s: %8lu\n", "Component count", components.size());
    printf("%-25s: %8lu\n", "Maximum component size", maxComponent.size());
    printf("%-25s: %8.3f\n", "Average distance", avgDistance);
    printf("%-25s: %8.3f\n", "Average degree", avgDegree);
}

void NetworkMatrix::failure_step()
{
    if (count(deleted, true) == this->rowCount)
        return;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, this->rowCount);
    assert(dist.min() == 0);
    assert(dist.max() == (int)this->rowCount);

    uint toDelete = dist(rd);

    while (deleted[toDelete])
        toDelete = dist(rd);

    deleted[toDelete] = true;
    for (uint col = 0; col < this->colCount; col++)
    {
        at(toDelete, col) = 0.0f;
        at(col, toDelete) = 0.0f;
    }
}

void NetworkMatrix::attack_step()
{
    uint toDelete = get_vertex_with_max_degree();
    assert(!deleted[toDelete]);

    deleted[toDelete] = true;
    for (uint col = 0; col < this->colCount; col++)
    {
        at(toDelete, col) = 0.0f;
        at(col, toDelete) = 0.0f;
    }
}