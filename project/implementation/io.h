#pragma once
#include "libs.h"
//std::vector<std::string> getNextLineAndSplitIntoTokens(std::istream& str)

std::vector<std::pair<uint, uint>> load_edge_pairs(const char *filename, const std::string &delimiter);
uint get_vertex_count_from_edge_pairs(const std::vector<std::pair<uint, uint>> &pairs);

#include "io.cpp"