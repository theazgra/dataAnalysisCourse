#include <networkLib/temporal_converter.h>

namespace azgra::networkLib
{
std::vector<std::vector<TemporalEdge>>
TemporalNetworkConverter::transform_temporal_edges_to_temporal_layers(const std::vector<TemporalEdge> &temporalEdges, const uint layerDuration)
{
    // We assert that temporal edges are sorted by time.
    size_t currentDurationEnd = layerDuration;
    std::vector<std::vector<TemporalEdge>> edgeLayers;

    std::vector<TemporalEdge> layer;
    for (const TemporalEdge &edge : temporalEdges)
    {
        if (edge.time > currentDurationEnd)
        {
            edgeLayers.push_back(std::vector<TemporalEdge>(layer));
            layer.clear();
            currentDurationEnd += layerDuration;
        }
        layer.push_back(edge);
    }
    if (layer.size() > 0)
        edgeLayers.push_back(layer);

    return edgeLayers;
}
}; // namespace azgra::networkLib
