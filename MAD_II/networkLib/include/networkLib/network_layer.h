#pragma once

#include <networkLib/network_matrix.h>
#include <string>

namespace azgra::networkLib
{

class NetworkLayer
{
private:
    std::string _name;
    NetworkMatrix _network;

public:
    NetworkLayer(const std::string &name, NetworkMatrix network);
    ~NetworkLayer();

    std::string &name();
    NetworkMatrix &network();
};

}; // namespace azgra::networkLib