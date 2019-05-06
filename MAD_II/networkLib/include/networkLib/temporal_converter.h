#pragma once
#include <networkLib/libs.h>
#include <networkLib/temporal_edge.h>

namespace azgra::networkLib
{
class TemporalNetworkConverter
{
public:
    static std::vector<std::vector<TemporalEdge>>
    transform_temporal_edges_to_temporal_layers(const std::vector<TemporalEdge> &temporalEdges, const uint layerDuration);
};
}; // namespace azgra::networkLib
