#include "network_matrix.h"

NetworkMatrix::NetworkMatrix(const char *fName, const int offset)
{
    auto loadedEdges = load_edge_pairs(fName, ";");
    uint vc = get_vertex_count_from_edge_pairs(loadedEdges);
    this->rowCount = vc;
    this->colCount = vc;

    this->data.resize(this->rowCount * this->colCount);
    set_matrix_to_one_value(0.0f);

    load_from_edges(loadedEdges, offset);
}

NetworkMatrix::NetworkMatrix(const uint &rowCount, const uint &colCount)
{
    this->rowCount = rowCount;
    this->colCount = colCount;
    this->data.resize(this->rowCount * this->colCount);
    set_matrix_to_one_value(0.0f);
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
    return this->rowCount;
}

uint NetworkMatrix::edge_count() const
{
    uint result = 0;

    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = 0; col < this->colCount; col++)
        {
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

    uint index = 0;
    std::vector<uint> neighbors;
    float neighboursEdgeCount;
    float nc;
    float clusteringCoeff;
    for (const uint &vertex : vertices)
    {
        neighbors.clear();
        neighbors = get_neighbours(vertex);

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

std::vector<uint> NetworkMatrix::get_neighbours(const uint vertex) const
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
    while (true)
    {
        current = q.front();
        q.pop();

        if (current == dest)
            break;

        neighbors = get_neighbours(current);
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
    auto neis = get_neighbours(current);
    assert(neis.size() > 0);
    for (const uint &neighbor : neis)
    {
        new_best_distance(neighbor, unvisited, distances, INFINITY);
    }

    while (!visited[dest])
    {
        current = get_best_unvisited(unvisited, distances);
        auto neighbors = get_neighbours(current);
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
    printf("Will use %s alg.\n", bfs ? "BreadthFirstSearch" : "Dijkstra");

    NetworkMatrix distanceMat(this->rowCount, this->colCount);
    NetworkMatrix result(this->rowCount, this->colCount);

    distanceMat.copy_data(*this);
    result.copy_data(*this);
    distanceMat.set_inf_where_no_edge();

#pragma omp parallel for
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = row + 1; col < this->colCount; col++)
        {
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
}
void NetworkMatrix::generate_scale_free_network(uint numberOfConnections, const uint numberOfVerticesToAdd)
{
    uint startingSize = 3;
    uint resultSize = startingSize + numberOfVerticesToAdd;
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

    std::vector<float> weights;
    std::vector<uint> neighbors;
    for (uint step = 0; step < numberOfVerticesToAdd; step++)
    {
        weights.clear();
        neighbors.clear();

        weights.reserve(currentSize);
        neighbors.reserve(numberOfConnections);

        newVertexIndex = startingSize + step;
        vertexListSize = (float)vertexList.size();

        for (uint vertex = 0; vertex < currentSize; vertex++)
        {
            weights.push_back(((float)count(vertexList, vertex) / vertexListSize));
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

    printf("VC: %u; EC: %u\n", vertex_count(), edge_count());
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
                auto lastConnectedNeighbors = get_neighbours(lastConnectedVertex);
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
                auto lastConnectedNeighbors = get_neighbours(lastConnectedVertex);
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

void NetworkMatrix::export_network(const char *filename) const
{
    std::vector<std::pair<uint, uint>> edges;
    for (uint row = 0; row < this->rowCount; row++)
    {
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
    uint count = 0;
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = row + 1; col < this->colCount; col++)
        {
            count++;
            distanceSum += distanceMatrix.at(row, col);
        }
    }

    float result = distanceSum / (float)((this->rowCount * (this->rowCount - 1)) / 2.0f);
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
            for (const uint n : get_neighbours(i))
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
            for (const uint n : get_neighbours(row))
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

uint NetworkMatrix::calculate_cut_size(const std::vector<uint> &gA, const std::vector<uint> &gB) const
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

    uint lastCut = calculate_cut_size(groupA, groupB);
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

                uint cut = calculate_cut_size(groupA, groupB);
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
        cut = calculate_cut_size(groupA, groupB);

        if (cut > lastCut)
        {
            groupA[bestSwap.aIndex] = vA;
            groupB[bestSwap.bIndex] = vB;
            cut = calculate_cut_size(groupA, groupB);
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