#pragma once
#include <networkLib/simple_string.h>

namespace azgra::networkLib
{

enum NetworkModel
{
    NetworkModel_None,
    NetworkModel_Random,
    NetworkModel_BarabasiAlbert,
    NetworkModel_HolmeKim,
    NetworkModel_Bianconi,
    NetworkModel_Copying,
    NetworkModel_LinkSelection
};

inline azgra::SimpleString get_network_model_name(const NetworkModel &model)
{
    switch (model)
    {
    case NetworkModel_Random:
        return "Random model";
    case NetworkModel_BarabasiAlbert:
        return "Barabasi-Albert model";
    case NetworkModel_Bianconi:
        return "Bianconi model";
    case NetworkModel_HolmeKim:
        return "Holme-Kim model";
    case NetworkModel_Copying:
        return "Copying model";
    case NetworkModel_LinkSelection:
        return "Link selection model";
    default:
        return "";
    }
}

struct GeneratorParameters
{
    bool isSet;
    NetworkModel model;
    uint initialSize;
    uint finalSize;
    float probability;
    float ageScaling;
    uint newEdgesInStep;
    bool aging;
    bool symmetric;

    GeneratorParameters()
    {
        isSet = false;
        model = NetworkModel_None;
        initialSize = 0;
        finalSize = 0;
        probability = 0.0f;
        ageScaling = 0.0f;
        newEdgesInStep = 0;
        aging = false;
        symmetric = false;
    }
};
}; // namespace azgra::networkLib