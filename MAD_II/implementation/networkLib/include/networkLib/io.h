#pragma once

#include <stdlib.h>
#include <networkLib/libs.h>
#include <networkLib/iris.h>
#include <networkLib/temporal_edge.h>
#include <networkLib/simple_string.h>
#include <networkLib/mpx_types.h>
#include <unordered_set>

namespace azgra::networkLib
{
void preprocess_ids(std::vector<std::pair<uint, uint>> &edges);

std::vector<std::pair<uint, uint>> load_edge_pairs(const char *filename, const std::string &delimiter, int &offset,
                                                   bool preprocessIds = false);
uint get_vertex_count_from_edge_pairs(const std::vector<std::pair<uint, uint>> &pairs);

std::vector<IrisRecord> read_iris_file(const char *filename);
TemporalImportResult load_temporal_edges(const char *fileName, const char separator);

MpxImportResult load_mpx_file(const char *fileName);

void save_edge_betweenness(const char *fileName, std::vector<UndirectedEdge> &edges);

void save_network(const char *filename, const std::vector<std::pair<uint, uint>> &data);

void save_vertex_community_ids(const char *filename, const std::vector<uint> &communityIds);

template <typename T>
void save_network_with_custom_attr(const char *filename, const std::vector<std::pair<uint, uint>> &data, const std::vector<T> &attr)
{
    assert(attr.size() == data.size());

    std::ofstream outStream(filename);
    size_t i = 0;
    for (const std::pair<uint, uint> &edge : data)
    {
        outStream << std::to_string(edge.first) << ";" << std::to_string(edge.second) << attr[i++] << std::endl;
    }
    outStream.flush();
}

}; // namespace azgra::networkLib