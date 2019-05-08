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
    MultiLayerNetwork() {}
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
            fprintf(stdout, "Layer: %s, VC: %u, EC: %u\n", layerName.c_str(), layer.network().vertex_count(), layer.network().edge_count());
            result.layers.push_back(layer);
        }

        return result;
    }
};

}; // namespace azgra::networkLib