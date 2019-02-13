#pragma once
#include "libs.h"
#include "iris.h"

std::vector<std::pair<uint, uint>> load_edge_pairs(const char *filename, const std::string &delimiter);
uint get_vertex_count_from_edge_pairs(const std::vector<std::pair<uint, uint>> &pairs);

void save_network(const char *filename, const std::vector<std::pair<uint, uint>> &data);
std::vector<IrisRecord> read_iris_file(const char *filename);
#include "io.cpp"