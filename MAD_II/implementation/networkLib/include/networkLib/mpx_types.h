#pragma once
#include <networkLib/types.h>
#include <networkLib/temporal_edge.h>
#include <string>

namespace azgra::networkLib
{
enum MpxParserSection
{
    Mpx_None,
    Mpx_Type,
    Mpx_Layers,
    Mpx_ActorAttributes,
    Mpx_Actors,
    Mpx_Edges
};

enum MpxLayerType
{
    MpxLt_Undirected,
    MpxLt_Directed,
};

struct MpxLayer
{
    std::string name;
    MpxLayerType type;
};

struct MpxImportResult
{
    bool success;
    std::vector<std::string> actors;
    std::vector<MpxLayer> layers;
    std::vector<TemporalEdge> edges;
};
}; // namespace azgra::networkLib