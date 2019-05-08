#include <networkLib/network_layer.h>

namespace azgra::networkLib
{
NetworkLayer::NetworkLayer() {}
NetworkLayer::NetworkLayer(const std::string &name, const std::vector<TemporalEdge> &edges)
{
    _name = name;
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

    _network = NetworkMatrix(stdEdges, 0);
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

std::string &NetworkLayer::name()
{
    return _name;
}

NetworkMatrix &NetworkLayer::network()
{
    return _network;
}

}; // namespace azgra::networkLib