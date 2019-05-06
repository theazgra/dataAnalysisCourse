#pragma once

#include <stdlib.h>
#include <networkLib/libs.h>
#include <networkLib/iris.h>
#include <networkLib/temporal_edge.h>
#include <networkLib/simple_string.h>

namespace azgra::networkLib
{

std::vector<std::pair<uint, uint>> load_edge_pairs(const char *filename, const std::string &delimiter, int &offset);
uint get_vertex_count_from_edge_pairs(const std::vector<std::pair<uint, uint>> &pairs);

void save_network(const char *filename, const std::vector<std::pair<uint, uint>> &data);
std::vector<IrisRecord> read_iris_file(const char *filename);
std::vector<TemporalEdge> load_temporal_edges(const char *fileName, const char separator);
}; // namespace azgra::networkLib