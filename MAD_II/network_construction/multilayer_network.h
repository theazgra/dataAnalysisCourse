#pragma once
#include "network_matrix.h"
#include <string>

class NetworkLayer
{
private:
    std::string _name;
    NetworkMatrix _network;

public:
    NetworkLayer();
    ~NetworkLayer();

    std::string &name() const;
    NetworkMatrix &network() const;
};

class MultiLayerNetwork
{
private:
    std::vector<NetworkLayer> layers;

public:
    MultiLayerNetwork();
    ~MultiLayerNetwork();
};
