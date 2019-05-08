#pragma once

#include <string>
#include <networkLib/network_matrix.h>
#include <networkLib/temporal_edge.h>

namespace azgra::networkLib
{

class NetworkLayer
{
private:
    std::string name;
    // Vertex id represent index of an actor in MultiLayer network.
    std::vector<uint> actorIds;
    NetworkMatrix network;

    std::vector<uint> get_distinct_ids_from_edges(const std::vector<TemporalEdge> &edges) const;

public:
    // To make std::vector happy.
    NetworkLayer();
    NetworkLayer(const std::string &name, const std::vector<TemporalEdge> &edges);

    ~NetworkLayer();

    std::string &get_name();
    NetworkMatrix &get_network();

    bool contains_actor(const uint &actor);
    /**
     * @brief For given actor find his neighbors.
     * 
     * @param actor For which actor to find neighbors.
     * @return std::vector<uint> Indexed returned all indexes of actors.
     */
    std::vector<uint> get_actor_neighbors(const uint &actor);

    void fix_actors(const std::vector<uint> &correctActors)
    {
        for (size_t aId = 0; aId < actorIds.size(); aId++)
        {
            uint currentActorId = actorIds[aId];
            uint newActorId = find_index(correctActors, currentActorId);
            actorIds[aId] = newActorId;
        }
    }
};

}; // namespace azgra::networkLib