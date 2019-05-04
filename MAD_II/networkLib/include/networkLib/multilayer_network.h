#pragma once
#include <networkLib/network_layer.h>

namespace azgra::networkLib
{
class MultiLayerNetwork
{
private:
    std::vector<NetworkLayer> _layers;

public:
    MultiLayerNetwork();
    ~MultiLayerNetwork();
};

};