#include <networkLib/network_matrix.h>

namespace azgra::networkLib
{

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
    uint vc = get_vertex_count_from_edge_pairs(loadedEdges) + offset;
    this->rowCount = vc;
    this->colCount = vc;

    this->data.resize(this->rowCount * this->colCount);

    set_matrix_to_one_value(0.0f);

    load_from_edges(loadedEdges, offset);
    initialize_deleted();
}

NetworkMatrix::NetworkMatrix(const std::vector<std::pair<uint, uint>> &edges, int offset)
{
    uint vc = get_vertex_count_from_edge_pairs(edges) + offset;
    this->rowCount = vc;
    this->colCount = vc;

    this->data.resize(this->rowCount * this->colCount);

    set_matrix_to_one_value(0.0f);

    load_from_edges(edges, offset);
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

        float similarity = 0.0f;

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

// NetworkMatrix::NetworkMatrix(const std::vector<IrisRecord> &vectorData)
// {
//     size_t size = vectorData.size();
//     assert(size > 0);

//     this->rowCount = size;
//     this->colCount = size;

//     this->data.resize(size * size);
//     set_matrix_to_one_value(0.0f);

//     for (uint row = 0; row < this->rowCount; row++)
//     {
//         for (uint col = row; col < this->colCount; col++)
//         {
//             if (row == col)
//             {
//                 at(row, col) = 1.0f;
//                 at(col, row) = 1.0f;
//             }
//             else
//             {
//                 float gaussSimilarity = vectorData[row].gauss_distance(vectorData[col]);
//                 at(row, col) = gaussSimilarity;
//                 at(col, row) = gaussSimilarity;
//             }
//         }
//     }
//     initialize_deleted();
// }

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

float &NetworkMatrix::at(const uint &row, const uint &col)
{
    return this->data[((row * this->colCount) + col)];
}

float &NetworkMatrix::at_vec(const std::vector<float> &vec, const uint &row, const uint &col)
{
    return data[((row * this->colCount) + col)];
}

float &NetworkMatrix::at(const uint &index)
{
    return this->data[index];
}

const float &NetworkMatrix::at(const uint &row, const uint &col) const
{
    return this->data[((row * this->colCount) + col)];
}

const float &NetworkMatrix::at(const uint &index) const
{
    return this->data[index];
}

bool NetworkMatrix::is_edge_at(const uint &row, const uint &col) const
{
    return (!is_infinity(row, col) && at(row, col) > 0.0f);
}

bool NetworkMatrix::is_infinity(const uint &row, const uint &col) const
{
    bool result = at(row, col) == INFINITY;
    return result;
}

uint NetworkMatrix::vertex_count() const
{
    assert(this->rowCount == this->colCount);
    return count(deleted, false);
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

uint NetworkMatrix::get_total_degree_of_community(const std::vector<uint> &c)
{
    uint result = 0;

    for (const uint &u : c)
    {
        for (uint col = 0; col < this->colCount; col++)
        {
            if (is_edge_at(u, col))
            {
                ++result;
            }
        }
    }
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

void NetworkMatrix::load_from_edges(const std::vector<std::pair<uint, uint>> &edges, int offset)
{
    uint rowIndex, colIndex;
    for (const std::pair<uint, uint> &edge : edges)
    {
        rowIndex = edge.first + offset;
        colIndex = edge.second + offset;

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

void NetworkMatrix::export_edge_betweenness(const char *fileName) const
{
    std::vector<UndirectedEdge> edges;
    edges.reserve((rowCount * rowCount) / 2);

    for (uint row = 0; row < rowCount; row++)
    {
        for (uint col = row + 1; col < colCount; col++)
        {
            UndirectedEdge edge = {row, col, at(row, col)};
            edges.push_back(edge);
        }
    }

    save_edge_betweenness(fileName, edges);
}

float NetworkMatrix::get_network_diameter(const NetworkMatrix &distanceMatrix) const
{
    float max = -1.0f;
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

float NetworkMatrix::get_average_distance(const NetworkMatrix &distanceMatrix) const
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

uint NetworkMatrix::get_edge_count_in_component(const std::vector<uint> &c)
{
    uint result = 0;

    for (const uint &u : c)
    {
        for (const uint &v : c)
        {
            if (u == v)
                continue;

            if (is_edge_at(u, v))
                ++result;
        }
    }

    return result;
}

void NetworkMatrix::delete_edges_for(const uint vertex)
{
    for (uint col = 0; col < this->colCount; col++)
    {
        at(vertex, col) = 0.0f;
        at(col, vertex) = 0.0f;
    }
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
    float avgDistance = get_average_distance(get_distance_matrix());
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

NetworkReport NetworkMatrix::get_network_report(const ReportRequest &request) const
{
    NetworkReport report = {};
    report.vertexCount = vertex_count();
    report.edgeCount = edge_count();
    report.averageDegree = get_average_degree();
    report.averageClusteringCoefficient = get_average_clustering_coefficient();

    auto distanceMatrix = get_distance_matrix();
    report.averageDistance = get_average_distance(distanceMatrix);
    report.networkDiameter = get_network_diameter(distanceMatrix);

    auto components = get_components();
    auto maxComponent = get_biggest_component(components);
    report.componentCount = components.size();
    report.maxComponentSize = maxComponent.size();

    if (request.includeVertexStats)
    {
        report.vertexDegrees = get_degree_of_vertices();
        report.vertexEccentricities = get_eccentricities(distanceMatrix);
        report.vertexClusteringCoefficients = get_clustering_coeff_for_all_vertices();
        report.vertexClosenessCentralities = get_closeness_centrality_for_vertices(distanceMatrix);
    }

    return report;
}

inline uint dp2_pick_best_unvisited(const std::vector<bool> &visited, const std::vector<float> &dist)
{
    uint result = 0;
    float min = INFINITY;

    for (size_t i = 0; i < visited.size(); i++)
    {
        if (!visited[i] && dist[i] <= min)
        {
            min = dist[i];
            result = i;
        }
    }

    //assert(min != INFINITY);
    return result;
}

void NetworkMatrix::dijkstra_distance(const NetworkMatrix &mat, const uint &source, NetworkMatrix &result) const
{
    // Total vertex count.
    size_t vertexCount = this->rowCount;

    std::vector<float> dist(vertexCount);
    std::vector<bool> visited(vertexCount);

    for (size_t v = 0; v < vertexCount; v++)
    {
        dist[v] = INFINITY;
        visited[v] = false;
    }
    dist[source] = 0;

    for (size_t count = 0; count < vertexCount - 1; count++)
    {
        uint u = dp2_pick_best_unvisited(visited, dist);
        visited[u] = true;

        // Update dist value of the adjacent vertices of the picked vertex.
        for (size_t v = 0; v < vertexCount; v++)
        {
            // Update dist[v] only if is not visited, there is an edge from
            // u to v, and total weight of path from source to v through u is
            // smaller than current value of dist[v].

            if (!visited[v] &&                        // v is not visited
                mat.is_edge_at(u, v) &&               // there is an edge from `u` to `v`
                ((dist[u] + mat.at(u, v)) < dist[v])) // distance from source to `v` through `u` is better than the one found
            {
                dist[v] = dist[u] + mat.at(u, v);
            }
        }
    }

    for (size_t v = source + 1; v < this->colCount; v++)
    {
        result.at(source, v) = dist[v];
        result.at(v, source) = dist[v];
    }
}

void NetworkMatrix::dijkstra_distance_with_path(const NetworkMatrix &mat, const uint &source, NetworkMatrix &result,
                                                std::vector<std::vector<UndirectedEdge>> &paths) const
{
    // Total vertex count.
    size_t vertexCount = this->rowCount;

    std::vector<float> dist(vertexCount);
    std::vector<bool> visited(vertexCount);
    std::vector<int> parents(vertexCount);

    for (size_t v = 0; v < vertexCount; v++)
    {
        dist[v] = INFINITY;
        visited[v] = false;
        parents[v] = -1;
    }
    dist[source] = 0;

    for (size_t count = 0; count < vertexCount - 1; count++)
    {
        uint u = dp2_pick_best_unvisited(visited, dist);
        visited[u] = true;

        // Update dist value of the adjacent vertices of the picked vertex.
        for (size_t v = 0; v < vertexCount; v++)
        {
            // Update dist[v] only if is not visited, there is an edge from
            // u to v, and total weight of path from source to v through u is
            // smaller than current value of dist[v].

            if (!visited[v] &&                        // v is not visited
                mat.is_edge_at(u, v) &&               // there is an edge from `u` to `v`
                ((dist[u] + mat.at(u, v)) < dist[v])) // distance from source to `v` through `u` is better than the one found
            {
                parents[v] = u;
                dist[v] = dist[u] + mat.at(u, v);
            }
        }
    }

    for (size_t v = source + 1; v < this->colCount; v++)
    {
        result.at(source, v) = dist[v];
        result.at(v, source) = dist[v];

        std::vector<uint> vPath;
        int current = v;
        do
        {
            vPath.push_back(current);
            current = parents[current];
        } while (current != -1);
        std::vector<UndirectedEdge> vEdgePath(vPath.size() - 1);

        for (size_t i = 0; i < vPath.size() - 1; i++)
        {
            UndirectedEdge edge(vPath[i], vPath[i + 1]);
            vEdgePath[i] = edge;
        }

        paths[v] = vEdgePath;
        // fprintf(stdout, "Shortest path [%u->%u]: %.0f CORRECT: %i | ", source, static_cast<uint>(v), dist[v], dist[v] == (vPath.size() - 1));
        // print_vector(vPath, "%u-");
    }
}

NetworkMatrix NetworkMatrix::get_edge_betweenness_matrix() const
{
    assert(this->rowCount == this->colCount);

    NetworkMatrix distanceMat(this->rowCount, this->colCount);
    NetworkMatrix calculatedDistanceMatrix(this->rowCount, this->colCount);

    distanceMat.copy_data(*this);
    calculatedDistanceMatrix.copy_data(*this);
    distanceMat.set_inf_where_no_edge();

    std::vector<std::vector<std::vector<UndirectedEdge>>> networkPaths;
    networkPaths.resize(this->rowCount);
#pragma omp parallel for
    for (uint row = 0; row < this->rowCount; row++)
    {
        std::vector<std::vector<UndirectedEdge>> rowPaths;
        rowPaths.resize(rowCount);
        dijkstra_distance_with_path(distanceMat, row, calculatedDistanceMatrix, rowPaths);
#pragma omp critical
        {
            networkPaths[row] = rowPaths;
        }
    }
    NetworkMatrix ccMatrix(this->rowCount, this->rowCount);
#pragma omp parallel for
    for (uint ebRow = 0; ebRow < this->rowCount; ebRow++)
    {
        for (uint ebCol = ebRow + 1; ebCol < this->rowCount; ebCol++)
        {
            // For edge (ebRow , ebCol), if there is
            if (is_edge_at(ebRow, ebCol))
            {
                UndirectedEdge testedEdge(ebRow, ebCol);
                uint throughEdge = 0;
                uint totalPathCount = 0;

                for (uint u = 0; u < this->rowCount; u++)
                {
                    for (uint v = u + 1; v < this->rowCount; v++)
                    {
                        ++totalPathCount;

                        // Get shortest path from u to v
                        std::vector<UndirectedEdge> uvPath = networkPaths[u][v];
                        if (find(uvPath, testedEdge))
                        {
                            ++throughEdge;
                        }
                    }
                }

                float EBC = static_cast<float>(throughEdge) / static_cast<float>(totalPathCount);
                ccMatrix.at(ebRow, ebCol) = EBC;
                ccMatrix.at(ebCol, ebRow) = EBC;
            }
        }
    }

    return ccMatrix;
} // namespace azgra::networkLib

NetworkMatrix NetworkMatrix::get_distance_matrix() const
{
    assert(this->rowCount == this->colCount);

    NetworkMatrix distanceMat(this->rowCount, this->colCount);
    NetworkMatrix result(this->rowCount, this->colCount);

    distanceMat.copy_data(*this);
    result.copy_data(*this);
    distanceMat.set_inf_where_no_edge();

#pragma omp parallel for
    for (uint row = 0; row < this->rowCount; row++)
    {
        dijkstra_distance(distanceMat, row, result);
    }
    return result;
}

uint NetworkMatrix::get_count_of_shared_neighbors(const uint u, const uint v) const
{
    return get_count_of_same_neighbors(get_neighbors(u), get_neighbors(v));
}
}; // namespace azgra::networkLib