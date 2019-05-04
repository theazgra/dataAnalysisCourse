#pragma once
#include <networkLib/network_matrix.h>
#include <string>

namespace azgra::networkLib
{
struct EpidemicIterationInfo
{
    uint time;
    std::vector<uint> suspected;
    std::vector<uint> infected;
    std::vector<uint> recovered;

    EpidemicIterationInfo() {}
    EpidemicIterationInfo(const uint _time) : time(_time) {}
};

namespace epidemic_impl
{
enum Model
{
    Model_SI,
    Model_SIS,
    Model_SIR,
};

enum State
{
    State_Suspected,
    State_Infected,
    State_Recovered
};

struct VertexState
{
    State state;
    uint infectionTime;
    bool immune = false;

    VertexState()
    {
        state = State_Suspected;
        infectionTime = 0;
    }
    VertexState(State _state)
    {
        state = _state;
        infectionTime = 0;
    }
};

/**
 * @brief Get initial infected vertices.
 * 
 * @param vertexCount Number of vertices in a network.
 * @param count Wanted count.
 * @return std::vector<uint> Infected vertices.
 */
static std::vector<uint> get_initial_infected(const uint vertexCount, const uint count)
{
    std::vector<uint> infected;
    infected.resize(count);

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> uniformIntRandom(0, (vertexCount - 1));

    for (size_t i = 0; i < count; i++)
    {
        uint vertex;
        do
        {
            vertex = uniformIntRandom(rd);
        } while (find(infected, vertex));
        infected[i] = vertex;
    }

    return infected;
}

static void reset_immune(std::vector<VertexState> &states)
{
    for (size_t i = 0; i < states.size(); i++)
    {
        states[i].immune = false;
    }
}

static EpidemicIterationInfo get_epidemic_iteration_info(const uint time, const std::vector<VertexState> &states)
{
    EpidemicIterationInfo result(time);
    for (size_t i = 0; i < states.size(); i++)
    {
        switch (states[i].state)
        {
        case State_Suspected:
            result.suspected.push_back(i);
            break;
        case State_Infected:
            result.infected.push_back(i);
            break;
        case State_Recovered:
            result.recovered.push_back(i);
            break;
        }
    }
    return result;
}

template <Model epidemiaModel>
static std::vector<EpidemicIterationInfo> epidemic_model(const NetworkMatrix &network,
                                                         const uint initialInfectedCount, const float infectionProbability,
                                                         const uint recoveryTime, const uint iterationCount)
{
    uint vertexCount = network.vertex_count();
    std::vector<VertexState> states;
    states.resize(vertexCount);

    for (size_t i = 0; i < vertexCount; i++)
        states[i] = VertexState();

    auto initialInfected = epidemic_impl::get_initial_infected(vertexCount, initialInfectedCount);
    for (const uint &v : initialInfected)
    {
        states[v].state = State_Infected;
        states[v].infectionTime = 0;
    }

    std::random_device rd;
    std::mt19937 mt(rd());
    std::discrete_distribution<int> infectionRandom({1 - infectionProbability, infectionProbability});

    std::vector<EpidemicIterationInfo> result;
    result.resize(iterationCount + 1);
    EpidemicIterationInfo initialInfo = get_epidemic_iteration_info(0, states);
    result[0] = initialInfo;

    for (uint time = 1; time < iterationCount + 1; time++)
    {
        // This switch statement gets resolved compile time.
        switch (epidemiaModel)
        {
        case Model_SI:
        {
            for (uint vertex = 0; vertex < vertexCount; vertex++)
            {
                if (states[vertex].state == State_Infected && states[vertex].infectionTime != time)
                {
                    if (infectionRandom(rd) == 1)
                    {
                        auto neighbors = network.get_neighbors(vertex);
                        for (const uint &neighbor : neighbors)
                        {
                            states[neighbor] = State_Infected;
                            states[neighbor].infectionTime = time;
                        }
                    }
                }
            }
        }
        break;
        case Model_SIS:
        {
            // First we will check for recovered nodes.
            for (uint vertex = 0; vertex < vertexCount; vertex++)
            {
                if (states[vertex].state == State_Infected && (time - states[vertex].infectionTime) >= recoveryTime)
                {
                    states[vertex].state = State_Suspected;
                    states[vertex].immune = true;
                }
            }

            for (uint vertex = 0; vertex < vertexCount; vertex++)
            {
                if (states[vertex].state == State_Infected && states[vertex].infectionTime != time)
                {
                    if (infectionRandom(rd) == 1)
                    {
                        auto neighbors = network.get_neighbors(vertex);
                        for (const uint &neighbor : neighbors)
                        {
                            if (states[neighbor].immune)
                                continue;

                            states[neighbor] = State_Infected;
                            states[neighbor].infectionTime = time;
                        }
                    }
                }
            }
            reset_immune(states);
        }
        break;
        case Model_SIR:
        {
            // First we will check for recovered nodes.
            for (uint vertex = 0; vertex < vertexCount; vertex++)
            {
                if (states[vertex].state == State_Infected && (time - states[vertex].infectionTime) >= recoveryTime)
                {
                    states[vertex].state = State_Recovered;
                    states[vertex].immune = true;
                }
            }

            for (uint vertex = 0; vertex < vertexCount; vertex++)
            {
                if (states[vertex].state == State_Infected && states[vertex].infectionTime != time)
                {
                    if (infectionRandom(rd) == 1)
                    {
                        auto neighbors = network.get_neighbors(vertex);
                        for (const uint &neighbor : neighbors)
                        {
                            if (states[neighbor].immune)
                                continue;

                            states[neighbor] = State_Infected;
                            states[neighbor].infectionTime = time;
                        }
                    }
                }
            }
        }
        break;
        }

        EpidemicIterationInfo itInfo = get_epidemic_iteration_info(time, states);
        result[time] = itInfo;
    }

    return result;
} // namespace epidemic_impl
}; // namespace epidemic_impl

inline std::vector<EpidemicIterationInfo> SI_epidemic_model(const NetworkMatrix &network, const uint initialInfectedCount,
                                                            const float infectionProbability, const uint iterationCount)
{
    return epidemic_impl::epidemic_model<epidemic_impl::Model_SI>(network, initialInfectedCount,
                                                                  infectionProbability, 0, iterationCount);
}

inline std::vector<EpidemicIterationInfo> SIS_epidemic_model(const NetworkMatrix &network, const uint initialInfectedCount,
                                                             const float infectionProbability, const uint recoveryTime,
                                                             const uint iterationCount)
{
    return epidemic_impl::epidemic_model<epidemic_impl::Model_SIS>(network, initialInfectedCount,
                                                                   infectionProbability, recoveryTime, iterationCount);
}

inline std::vector<EpidemicIterationInfo> SIR_epidemic_model(const NetworkMatrix &network, const uint initialInfectedCount,
                                                             const float infectionProbability, const uint recoveryTime,
                                                             const uint iterationCount)
{
    return epidemic_impl::epidemic_model<epidemic_impl::Model_SIR>(network, initialInfectedCount,
                                                                   infectionProbability, recoveryTime, iterationCount);
}

static void print_epidemic_stats(const std::vector<EpidemicIterationInfo> &stats)
{
    for (size_t time = 0; time < stats.size(); time++)
    {
        uint itTime = stats[time].time;
        size_t suspectedCount = stats[time].suspected.size();
        size_t infectedCount = stats[time].infected.size();
        size_t recoveredCount = stats[time].recovered.size();

        printf("Epidemic time: %u\n", itTime);
        printf("\t%-20s:%5lu\n", "Suspected count", suspectedCount);
        printf("\t%-20s:%5lu\n", "Infected count", infectedCount);
        printf("\t%-20s:%5lu\n", "Recovered count", recoveredCount);
    }
}

static void save_epidemic_stats(const std::vector<EpidemicIterationInfo> &stats, const char *_folder)
{
    std::string folder = _folder;

    for (size_t time = 0; time < stats.size(); time++)
    {
        uint itTime = stats[time].time;

        std::string file = folder + "/" + "iteration_" + std::to_string(itTime) + ".csv";
        std::ofstream outStream(file, std::ios::out);
        if (!outStream.is_open())
        {
            printf("Unable to open out stream to file %s.\n", file.c_str());
            return;
        }
        outStream << "Suspected;Infected;Recovered" << std::endl;

        size_t suspectedCount = stats[time].suspected.size();
        size_t infectedCount = stats[time].infected.size();
        size_t recoveredCount = stats[time].recovered.size();
        size_t max = suspectedCount >= infectedCount ? suspectedCount : infectedCount;
        max = max > recoveredCount ? max : recoveredCount;

        for (size_t i = 0; i < max; i++)
        {
            if (i < suspectedCount)
                outStream << stats[time].suspected[i];

            outStream << ";";

            if (i < infectedCount)
                outStream << stats[time].infected[i];

            outStream << ";";

            if (i < recoveredCount)
                outStream << stats[time].recovered[i];

            outStream << std::endl;
        }
    }
}
};