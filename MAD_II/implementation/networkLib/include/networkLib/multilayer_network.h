#pragma once
#include <networkLib/network_layer.h>

namespace azgra::networkLib
{
template <typename TActorType>
class MultiLayerNetwork
{
private:
    std::vector<TActorType> actors;
    std::vector<NetworkLayer> layers;

public:
    MultiLayerNetwork()
    {
    }
    ~MultiLayerNetwork() {}

    static MultiLayerNetwork<TActorType> import_from_mpx(const char *fileName)
    {
        MpxImportResult mpx = load_mpx_file(fileName);
        assert(mpx.success);
        MultiLayerNetwork<TActorType> result;
        result.actors = mpx.actors;

        for (size_t layerId = 0; layerId < mpx.layers.size(); layerId++)
        {
            std::vector<TemporalEdge> layerEdges;
            for (size_t edgeId = 0; edgeId < mpx.edges.size(); edgeId++)
            {
                if (mpx.edges[edgeId].time == layerId)
                    layerEdges.push_back(mpx.edges[edgeId]);
            }
            NetworkLayer layer(mpx.layers[layerId].name, layerEdges);
            result.layers.push_back(layer);
        }

        return result;
    }

    static MultiLayerNetwork<TActorType> import_from_temporal_data(const char *fileName, const char separator, const uint temporalLayerDuration)
    {
        TemporalImportResult tir = load_temporal_edges(fileName, separator);

        //NOTE: We assome that temporal edges are sorted by time.
        size_t currentDurationEnd = temporalLayerDuration;
        std::vector<std::vector<TemporalEdge>> edgeLayers;

        std::vector<TemporalEdge> layer;
        for (const TemporalEdge &edge : tir.edges)
        {
            if (edge.time > currentDurationEnd)
            {
                edgeLayers.push_back(std::vector<TemporalEdge>(layer));
                layer.clear();
                currentDurationEnd += temporalLayerDuration;
            }
            layer.push_back(edge);
        }
        if (layer.size() > 0)
            edgeLayers.push_back(layer);

        fprintf(stdout, "Sorted into %lu temporal layers\n", edgeLayers.size());

        MultiLayerNetwork<TActorType> result;
        result.actors = tir.actors;

        for (size_t layerId = 0; layerId < edgeLayers.size(); layerId++)
        {
            std::string layerName = "Temporal_" +
                                    std::to_string(layerId * temporalLayerDuration) +
                                    "_" + std::to_string(layerId * temporalLayerDuration + temporalLayerDuration);

            NetworkLayer layer(layerName, edgeLayers[layerId]);
            layer.fix_actors(result.actors);

            //fprintf(stdout, "Layer: %s, VC: %u, EC: %u\n", layerName.c_str(), layer.get_network().vertex_count(), layer.get_network().edge_count());
            result.layers.push_back(layer);
        }

        return result;
    }

    /**
     * @brief Get neighborhood centrality, Number of connected actors for every actor in selected layers.
     * 
     * @param layerIds Layers to include in analysis.
     * @return std::map<TActorType, uint> Map of actor and count of his neigboring actors in selected layers.
     */
    std::map<TActorType, uint> get_actors_neighborhood_centrality(const std::vector<uint> &layerIds)
    {
        std::map<TActorType, uint> result;
        std::map<TActorType, std::vector<TActorType>> actorNeighbors = get_actors_neighbors(layerIds);
        for (const std::pair<TActorType, std::vector<TActorType>> &kvPair : actorNeighbors)
        {
            result[kvPair.first] = static_cast<uint>(kvPair.second.size());
        }
        return result;
    }

    std::map<TActorType, uint> get_actors_exclusive_neighborhood_centrality(const std::vector<uint> &layerIds)
    {
        std::map<TActorType, uint> result;
        std::map<TActorType, std::vector<TActorType>> actorNeighbors = get_actors_neighbors(layerIds);
        std::map<TActorType, std::vector<TActorType>> actorNeighborsExcept = get_actors_neighbors(except(get_all_layers_ids(), layerIds));

        for (size_t actorId = 0; actorId < actors.size(); actorId++)
        {
            const TActorType actor = actors[actorId];
            std::vector<TActorType> diff = except(actorNeighbors[actor], actorNeighborsExcept[actor]);
            result[actor] = static_cast<uint>(diff.size());
        }
        return result;
    }
    /**
     * @brief Get actor neighbors, in selected layers.
     * 
     * @param layerIds Layers to include in analysis
     * @return std::map<TActorType, std::vector<TActorType>> 
     */
    std::map<TActorType, std::vector<TActorType>> get_actors_neighbors(const std::vector<uint> &layerIds)
    {
        std::map<TActorType, std::vector<TActorType>> result;

        for (size_t actorId = 0; actorId < actors.size(); actorId++)
        {
            const TActorType actor = actors[actorId];

            std::vector<TActorType> actorNeighbors;
            for (const uint layerId : layerIds)
            {
                if (layers[layerId].contains_actor(actorId))
                {
                    std::vector<uint> neighboringActors = layers[layerId].get_actor_neighbors(actorId);
                    for (const uint &actorId : neighboringActors)
                    {
                        const TActorType neighbor = actors[actorId];
                        push_back_if_not_found(actorNeighbors, neighbor);
                    }
                }
            }
            result[actor] = actorNeighbors;
        }
        return result;
    }

    /**
     * @brief Get degree centrality, total number of edges for all actors in selected layers.
     * 
     * @param layerIds Layers to include in analysis.
     * @return std::map<TActorType, uint> Map of actor and his total edge count in selected layers.
     */
    std::map<TActorType, uint> get_actors_degree_centrality(const std::vector<uint> &layerIds)
    {
        std::map<TActorType, uint> result;

        for (size_t actorId = 0; actorId < actors.size(); actorId++)
        {
            const TActorType actor = actors[actorId];
            uint actorDegree = 0;
            for (const uint layerId : layerIds)
            {
                if (layers[layerId].contains_actor(actorId))
                {
                    std::vector<uint> neighboringActors = layers[layerId].get_actor_neighbors(actorId);
                    actorDegree += static_cast<uint>(neighboringActors.size());
                }
            }
            result[actor] = actorDegree;
        }
        return result;
    }

    /**
     * @brief Get connective redundacy for actors in selected layers.
     * 
     * @param layerIds Selected layers.
     * @return std::map<TActorType, float> Map of actor and his connective redundancy.
     */
    std::map<TActorType, float> get_actors_connective_redundancy(const std::vector<uint> &layerIds)
    {
        auto neighborhoodCentrality = get_actors_neighborhood_centrality(layerIds);
        auto degreeCentrality = get_actors_degree_centrality(layerIds);

        std::map<TActorType, float> result;
        for (size_t actorId = 0; actorId < actors.size(); actorId++)
        {
            const TActorType actor = actors[actorId];
            float redundancy = 1.0f - (static_cast<float>(neighborhoodCentrality[actor]) / static_cast<float>(degreeCentrality[actor]));
            result[actor] = redundancy;
        }
        return result;
    }

    /**
     * @brief Get ids of all layers.
     * 
     * @return std::vector<uint> Ids of all layers.
     */
    std::vector<uint> get_all_layers_ids() const
    {
        std::vector<uint> result(layers.size());
        for (size_t i = 0; i < layers.size(); i++)
        {
            result[i] = i;
        }

        return result;
    }
};

}; // namespace azgra::networkLib