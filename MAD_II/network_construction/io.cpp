#include "io.h"

std::vector<std::pair<uint, uint>> load_edge_pairs(const char *filename, const std::string &delimiter)
{
    std::vector<std::pair<uint, uint>> result;
    std::ifstream inStream(filename);

    assert(inStream.is_open());

    std::string line, token;
    size_t pos;
    std::string v1, v2;
    while (std::getline(inStream, line))
    {
        pos = line.find(delimiter);
        v1 = line.substr(0, pos);
        v2 = line.substr(pos + 1);

        result.push_back(std::make_pair((uint)std::stoi(v1), (uint)std::stoi(v2)));
    }
    return result;
}

uint get_vertex_count_from_edge_pairs(const std::vector<std::pair<uint, uint>> &pairs)
{
    uint max = 0;

    for (const std::pair<uint, uint> &edge : pairs)
    {
        if (edge.first > max)
            max = edge.first;
        if (edge.second > max)
            max = edge.second;
    }
    return max;
}

void save_network(const char *filename, const std::vector<std::pair<uint, uint>> &data)
{
    std::ofstream outStream(filename);

    for (const std::pair<uint, uint> &edge : data)
    {
        outStream << std::to_string(edge.first) << ";" << std::to_string(edge.second) << std::endl;
    }
    outStream.flush();
}

std::vector<IrisRecord> read_iris_file(const char *filename)
{
    std::vector<IrisRecord> result;
    std::ifstream inStream(filename);

    assert(inStream.is_open());

    std::string line, name;
    size_t lastPos, pos;
    std::string v1, v2;
    float pl, pw, sl, sw;

    // Skip first line
    std::getline(inStream, line);

    while (std::getline(inStream, line))
    {
        //sepal_length,sepal_width,petal_length,petal_width,species
        //5.1,3.5,1.4,0.2,setosa
        pos = line.find(',');
        sl = std::stof(line.substr(0, pos));
        lastPos = pos + 1;

        pos = line.find(',', lastPos);
        sw = std::stof(line.substr(lastPos, pos));
        lastPos = pos + 1;

        pos = line.find(',', lastPos);
        pl = std::stof(line.substr(lastPos, pos));
        lastPos = pos + 1;

        pos = line.find(',', lastPos);
        pw = std::stof(line.substr(lastPos, pos));
        lastPos = pos + 1;

        pos = line.find(',', lastPos);
        name = line.substr(lastPos, pos);

        result.push_back(IrisRecord(sl, sw, pl, pw, name.c_str()));
        printf("%f %f %f %f \n", sl, sw, pl, pw);
    }

    return result;
}