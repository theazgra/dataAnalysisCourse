#include "io.h"

std::vector<std::pair<uint, uint>> load_edge_pairs(const char *filename, const std::string &delimiter)
{
    std::vector<std::pair<uint, uint>> result;
    std::ifstream inStream(filename);

    assert(inStream.is_open());

    std::string line, token;
    size_t pos;
    std::string v1, v2;
    while (std::getline(inStream, line))
    {
        pos = line.find(delimiter);
        v1 = line.substr(0, pos);
        v2 = line.substr(pos + 1);

        result.push_back(std::make_pair((uint)std::stoi(v1), (uint)std::stoi(v2)));
    }
    return result;
}

uint get_vertex_count_from_edge_pairs(const std::vector<std::pair<uint, uint>> &pairs)
{
    uint max = 0;

    for (const std::pair<uint, uint> &edge : pairs)
    {
        if (edge.first > max)
            max = edge.first;
        if (edge.second > max)
            max = edge.second;
    }
    return max;
}