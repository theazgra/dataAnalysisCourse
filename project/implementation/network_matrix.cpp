#include "network_matrix.h"

NetworkMatrix::NetworkMatrix(const uint &rowCount, const uint &colCount)
{
    this->rowCount = rowCount;
    this->colCount = colCount;
    this->data.reserve(this->rowCount * this->colCount);
    set_matrix_to_one_value(0.0f);
}

void NetworkMatrix::reinitialize(const uint &rowCount, const uint &colCount)
{
    this->rowCount = rowCount;
    this->colCount = colCount;
    this->data.clear();
    this->data.shrink_to_fit();
    this->data.reserve(this->rowCount * this->colCount);
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
    return result;
}
NetworkMatrix NetworkMatrix::operator+(const NetworkMatrix &other)
{
    assert(this->rowCount == other.rows() && this->colCount == other.cols());
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = 0; col < this->colCount; col++)
        {
            at(row, col) += other.at(row, col);
        }
    }
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
}

NetworkMatrix NetworkMatrix::operator-(const NetworkMatrix &other)
{
    assert(this->rowCount == other.rows() && this->colCount == other.cols());
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = 0; col < this->colCount; col++)
        {
            at(row, col) -= other.at(row, col);
        }
    }
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
    std::vector<uint> neighbours;
    float neighboursEdgeCount;
    float nc;
    float clusteringCoeff;
    for (const uint &vertex : vertices)
    {
        neighbours.clear();
        neighbours = get_neighbours(vertex);

        if (neighbours.size() > 1)
        {
            nc = neighbours.size();
            neighboursEdgeCount = (float)get_edge_count_between_neighbours(neighbours);
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

uint NetworkMatrix::get_edge_count_between_neighbours(const std::vector<uint> &neighbours) const
{
    uint result = 0;
    for (const uint &n1Index : neighbours)
    {
        for (const uint &n2Index : neighbours)
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
uint get_best_unvisited(const std::vector<uint> &unvisited, const std::vector<float> &distances)
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
        return vertex;
    }
}

void new_best_distance(const uint &currentVertex, std::vector<uint> &unvisited, std::vector<float> &distances, const float distance)
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

    std::vector<uint> neighbours;
    while (true)
    {
        current = q.front();
        q.pop();

        if (current == dest)
            break;

        neighbours = get_neighbours(current);
        for (const uint &neighbour : neighbours)
        {
            if (!visited[neighbour])
            {
                q.push(neighbour);
                visited[neighbour] = true;
                previous[neighbour] = current;
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
        unvisited.push_back(v);
        distances.push_back(INFINITY);
        visited.push_back(false);
    }
    uint current = source;
    new_best_distance(current, unvisited, distances, 0);
    for (const uint &neighbour : get_neighbours(current))
    {
        new_best_distance(neighbour, unvisited, distances, INFINITY);
    }
    bool destVisited = false;
    while (!destVisited)
    {
        current = get_best_unvisited(unvisited, distances);
        for (const uint &neighbour : get_neighbours(current))
        {
            if (visited[neighbour])
                continue;
            float distanceToNeighbour = distances[current] + mat.at(current, neighbour);
            if (distanceToNeighbour < distances[neighbour])
            {
                distances[neighbour] = distanceToNeighbour;
            }
        }
        if (current == dest)
            break;

        visited[current] = true;

        unvisited.erase(std::remove(std::begin(unvisited), std::end(unvisited), current));
        assert(!find(unvisited, current));
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
        cc = 1.0f / distanceSum;
        result.push_back(cc);
    }

    return result;
}
// for rId in range(0, self.rowCount):
//             result.append(round(1 / (sum([self.values[rId][cId] for cId in range(0, self.colCount) if cId != rId]) / vertexCount), 7))

NetworkMatrix NetworkMatrix::get_distance_matrix() const
{
    assert(this->rowCount == this->colCount);
    bool bfs = can_use_bfs();
    printf("Will use %s alg.\n", bfs ? "BFS" : "DIJKSTRA");

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
void NetworkMatrix::generate_scale_free_network(const uint numberOfConnections, const uint numberOfVerticesToAdd)
{
    uint startingSize = 3;
    uint resultSize = startingSize + numberOfVerticesToAdd;
    uint currentSize = startingSize;

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
    std::vector<uint> vertexList = {0, 0, 1, 1, 2, 2};

    reinitialize(resultSize, resultSize);
    insert(initialMat);

    uint newVertexIndex, neighbour;
    float vertexListSize;

    std::random_device randomDevice;
    std::mt19937 randomGenerator(randomDevice());
    std::discrete_distribution<int> discreteDistribution;

    std::vector<float> weights;
    std::vector<uint> neighbours;
    for (uint step = 0; step < numberOfVerticesToAdd; step++)
    {
        weights.clear();
        neighbours.clear();

        weights.reserve(currentSize);
        neighbours.reserve(numberOfConnections);

        newVertexIndex = startingSize + step;
        vertexListSize = (float)vertexList.size();

        for (uint vertex = 0; vertex < currentSize; vertex++)
        {
            weights.push_back(((float)count(vertexList, vertex) / vertexListSize));
        }

        discreteDistribution = std::discrete_distribution<int>(std::begin(weights), std::end(weights));
        for (uint neighbourStep = 0; neighbourStep < numberOfConnections; neighbourStep++)
        {
            neighbour = discreteDistribution(randomGenerator);
            while (find(neighbours, neighbour))
            {
                neighbour = discreteDistribution(randomGenerator);
            }
            neighbours.push_back(neighbour);
        }

        assert(neighbours.size() == numberOfConnections);

        vertexList.push_back(newVertexIndex);
        vertexList.push_back(newVertexIndex);

        for (const uint &newNeighbour : neighbours)
        {
            vertexList.push_back(newNeighbour);
            at(newVertexIndex, newNeighbour) = 1.0;
            at(newNeighbour, newVertexIndex) = 1.0;
        }

        currentSize++;
    }
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

void NetworkMatrix::export_network(const char *filename) const
{
    std::vector<std::pair<uint, uint>> edges;
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = row; col < this->colCount; col++)
        {
            if (!is_infinity(row, col) && (at(row, col) > 0.0))
            {
                edges.push_back(std::make_pair(row, col));
            }
        }
    }
    save_network(filename, edges);
}