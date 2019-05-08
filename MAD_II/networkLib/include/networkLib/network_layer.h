#pragma once

#include <string>
#include <networkLib/network_matrix.h>
#include <networkLib/temporal_edge.h>

namespace azgra::networkLib
{

class NetworkLayer
{
private:
    std::string _name;
    // Vertex id represent index of an actor in MultiLayer network.
    std::vector<uint> actorIds;
    NetworkMatrix _network;

    std::vector<uint> get_distinct_ids_from_edges(const std::vector<TemporalEdge> &edges) const;

public:
    // To make std::vector happy.
    NetworkLayer();
    NetworkLayer(const std::string &name, const std::vector<TemporalEdge> &edges);
    ~NetworkLayer();

    std::string &name();
    NetworkMatrix &network();
};

}; // namespace azgra::networkLib