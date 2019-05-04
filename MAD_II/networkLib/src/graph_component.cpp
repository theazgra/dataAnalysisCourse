#include <networkLib/graph_component.h>

namespace azgra::networkLib
{
GraphComponent get_biggest_component(const std::vector<GraphComponent> &components)
{
    size_t maxSize = 0;
    GraphComponent max = {};
    for (const GraphComponent &component : components)
    {
        if (component.size() > maxSize)
        {
            maxSize = component.size();
            max = component;
        }
    }
    return max;
}
};