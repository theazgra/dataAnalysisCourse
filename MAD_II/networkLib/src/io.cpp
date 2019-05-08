#include <networkLib/io.h>
namespace azgra::networkLib
{
std::vector<std::pair<uint, uint>> load_edge_pairs(const char *filename, const std::string &delimiter, int &offset)
{
    offset = 0;
    uint min = 99999999;

    std::vector<std::pair<uint, uint>> result;

    std::ifstream inStream(filename);
    assert(inStream.is_open());

    std::string line, token;
    size_t pos;
    std::string v1, v2;
    uint u, v;

    while (std::getline(inStream, line))
    {
        pos = line.find(delimiter);
        v1 = line.substr(0, pos);
        v2 = line.substr(pos + 1);

        u = (uint)std::stoi(v1);
        v = (uint)std::stoi(v2);

        min = (u < min ? u : (v < min ? v : min));

        result.push_back(std::make_pair(u, v));
    }
    if (min != 0)
    {
        offset = -min;
        printf("Offset of %i\n", offset);
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
    return (max + 1);
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
    }

    return result;
}

TemporalImportResult load_temporal_edges(const char *fileName, const char separator)
{
    TemporalImportResult result = {};

    std::ifstream inStream(fileName);

    assert(inStream.is_open());
    if (!inStream.is_open())
        return result;

    std::string stdLine;

    while (std::getline(inStream, stdLine))
    {
        SimpleString line(stdLine.c_str());
        auto tokens = line.split(separator);
        if (tokens.size() == 3)
        {
            TemporalEdge edge = {};
            edge.time = static_cast<uint>(strtol(tokens[0]->get_c_string(), (char **)nullptr, 10));
            edge.u = static_cast<uint>(strtol(tokens[1]->get_c_string(), (char **)nullptr, 10));
            edge.v = static_cast<uint>(strtol(tokens[2]->get_c_string(), (char **)nullptr, 10));

            push_back_if_not_found(result.actors, edge.u);
            push_back_if_not_found(result.actors, edge.v);

            result.edges.push_back(edge);
        }
    }
    return result;
}

MpxImportResult load_mpx_file(const char *fileName)
{
    MpxImportResult result = {};
    result.success = false;

    std::ifstream inStream(fileName);
    assert(inStream.is_open());
    if (!inStream.is_open())
        return result;

    std::string stdLine;
    MpxParserSection section = Mpx_None;
    while (std::getline(inStream, stdLine))
    {
        if (stdLine.length() == 0 || stdLine == "\r")
            continue;

        SimpleString line(stdLine.c_str());
        line.remove('\r');
        // Section start.
        if (line.starts_with('#'))
        {
            if (line.starts_with("#TYPE"))
            {
                section = Mpx_Type;
                fprintf(stdout, "MpxType: %s\n", line.substring(6).get_c_string());
            }
            else if (line.starts_with("#LAYERS"))
            {
                section = Mpx_Layers;
            }
            else if (line.starts_with("#ACTOR ATTRIBUTES"))
            {
                section = Mpx_ActorAttributes;
            }
            else if (line.starts_with("#ACTORS"))
            {
                section = Mpx_Actors;
            }
            else if (line.starts_with("#EDGES"))
            {
                section = Mpx_Edges;
            }
        }
        else
        {
            switch (section)
            {
            case Mpx_Layers:
            {
                auto tokens = line.split(',');
                if (tokens.size() == 2)
                {
                    MpxLayer layer = {};
                    layer.name = tokens[0]->get_c_string();
                    layer.type = tokens[1]->equals("UNDIRECTED") ? MpxLt_Undirected : MpxLt_Directed;
                    result.layers.push_back(layer);
                }
                else
                {
                    fprintf(stderr, "Ignoring layer line %s\n", line.get_c_string());
                }
            }
            break;
            case Mpx_Actors:
            {
                int index = line.index_of(',');
                if (index > 0)
                {
                    std::string actorName = line.substring(0, index).get_c_string();
                    result.actors.push_back(actorName);
                }
                else
                {
                    fprintf(stderr, "Ignoring actor line %s\n", line.get_c_string());
                }
            }
            break;
            case Mpx_Edges:
            {
                auto edgeTokens = line.split(',');
                if (edgeTokens.size() == 3)
                {
                    int u = find_index(result.actors, std::string(edgeTokens[0]->get_c_string()));
                    int v = find_index(result.actors, std::string(edgeTokens[1]->get_c_string()));

                    int layerId = -1;
                    for (size_t id = 0; id < result.layers.size(); id++)
                    {
                        if (result.layers[id].name == std::string(edgeTokens[2]->get_c_string()))
                        {
                            layerId = id;
                            break;
                        }
                    }

                    if (u == -1 || v == -1 || layerId == -1)
                    {
                        fprintf(stderr, "Ignoring edge line %s\n", line.get_c_string());
                    }
                    else
                    {
                        TemporalEdge edge = {};
                        edge.u = u;
                        edge.v = v;
                        edge.time = layerId;
                        result.edges.push_back(edge);
                    }
                }
                else
                {
                    fprintf(stderr, "Ignoring edge line %s\n", line.get_c_string());
                }
            }
            break;

            default:
            {
                fprintf(stdout, "Ignoring line: %s\n", line.get_c_string());
            }
            break;
            }
        }
    }
    fprintf(stdout, "Completed import of:\n%5lu layers\n%5lu actors\n%5lu edges\n",
            result.layers.size(), result.actors.size(), result.edges.size());
    result.success = true;
    return result;
}
}; // namespace azgra::networkLib