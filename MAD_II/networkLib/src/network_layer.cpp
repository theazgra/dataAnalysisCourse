#include <networkLib/network_layer.h>

namespace azgra::networkLib
{

NetworkLayer::NetworkLayer(const std::string &name, NetworkMatrix network)
{
    _name = name;
    _network = network;
}

NetworkLayer::~NetworkLayer()
{
}

std::string &NetworkLayer::name()
{
    return _name;
}

NetworkMatrix &NetworkLayer::network()
{
    return _network;
}

};