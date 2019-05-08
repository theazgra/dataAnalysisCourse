#pragma once
#include <networkLib/types.h>
#include <vector>
namespace azgra::networkLib
{
struct TemporalEdge
{
    uint u;
    uint v;
    uint time;
};

struct TemporalImportResult
{
    std::vector<TemporalEdge> edges;
    std::vector<uint> actors;
};
}; // namespace azgra::networkLib