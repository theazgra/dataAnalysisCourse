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
        for (uint col = 0; col < this->colCount; col++)
        {
            at(row, col) = value;
        }
    }
}

void NetworkMatrix::transpose()
{
    for (uint row = 0; row < this->rowCount; row++)
    {
        for (uint col = 0; col < this->colCount; col++)
        {
            at(col, row) = at(row, col);
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

NetworkMatrix NetworkMatrix::get_distance_matrix() const
{
    //TODO: Rework to  BFS or DFS, this is really bad implementation.
    // https://github.com/theazgra/aMAZEing/blob/9711facd1baba625faec34373604f8d8be2ad3bf/aMaze_ingSolver/aMaze_ingSolver/Algorithms/BreadthFirst.cs#L274
    assert(this->rowCount == this->colCount);

    NetworkMatrix result = NetworkMatrix(this->rowCount, this->colCount);
    result.copy_data(*this);
    uint upperBound = this->rowCount;

    float tmpDistance;
    for (uint k = 0; k < upperBound; k++)
    {
        for (uint i = 0; i < upperBound; i++)
        {

            for (uint j = 0; j < upperBound; j++)
            {
                tmpDistance = result.at(i, k) + result.at(k, j);
                if (result.at(i, j) > tmpDistance)
                    result.at(i, k) = tmpDistance;
            }
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

void NetworkMatrix::generate_random_network(const uint vertexCount, const float edgeProbability, bool autoProbability)
{
    float prob = edgeProbability;
    if (autoProbability)
    {
        prob = get_probability_for_symmetric_network(vertexCount);
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