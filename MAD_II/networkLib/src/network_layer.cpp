#include <networkLib/network_layer.h>

namespace azgra::networkLib
{
NetworkLayer::NetworkLayer() {}
NetworkLayer::NetworkLayer(const std::string &name, const std::vector<TemporalEdge> &edges)
{
    this->name = name;
    actorIds = get_distinct_ids_from_edges(edges);

    std::vector<std::pair<uint, uint>> stdEdges;
    stdEdges.reserve(edges.size());

    for (size_t edgeId = 0; edgeId < edges.size(); edgeId++)
    {
        int uIndex = find_index(actorIds, edges[edgeId].u);
        int vIndex = find_index(actorIds, edges[edgeId].v);

        assert(uIndex != -1 && vIndex != -1);

        std::pair<uint, uint> stdEdge = std::make_pair(uIndex, vIndex);
        if (!find(stdEdges, stdEdge))
        {
            stdEdges.push_back(stdEdge);
        }
    }

    network = NetworkMatrix(stdEdges, 0);
}

NetworkLayer::~NetworkLayer()
{
}

std::vector<uint> NetworkLayer::get_distinct_ids_from_edges(const std::vector<TemporalEdge> &edges) const
{
    std::vector<uint> result;

    for (const TemporalEdge &e : edges)
    {
        push_back_if_not_found(result, e.u);
        push_back_if_not_found(result, e.v);
    }

    return result;
}

std::string &NetworkLayer::get_name()
{
    return name;
}

NetworkMatrix &NetworkLayer::get_network()
{
    return network;
}

bool NetworkLayer::contains_actor(const uint &actor)
{
    return find(actorIds, actor);
}

std::vector<uint> NetworkLayer::get_actor_neighbors(const uint &actor)
{
    int actorVertex = find_index(actorIds, actor);
    std::vector<uint> result;

    if (actorVertex == -1)
        return result;

    auto neighbors = network.get_neighbors(actorVertex);
    result.resize(neighbors.size());

    for (size_t nId = 0; nId < neighbors.size(); nId++)
    {
        result[nId] = actorIds[neighbors[nId]];
    }
    return result;
}

}; // namespace azgra::networkLib