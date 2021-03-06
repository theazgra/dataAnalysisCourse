#pragma once
#include <QDebug>
#include <networkLib/network_generator.h>
#include <networkLib/community.h>
#include <sstream>
#include <networkLib/Stopwatch.h>
#include <epidemic_dialog.h>

using namespace azgra::networkLib;

enum Algorithm
{
    Alg_GirvanNewman,
    Alg_Epidemic
};


struct AlgorithmResult
{
    Algorithm alg;
    QString log;
    CommunityEvolveArray communityEvolveResult;
    double elapsedMilliseconds;

    AlgorithmResult()
    {
    }
    AlgorithmResult(Algorithm _alg) : alg(_alg)
    {
    }
};

inline NetworkMatrix import_network_from_edges(const QString &fileName, const QString &delimiter, const bool preprocess)
{
    NetworkMatrix result(fileName.toStdString().c_str(), delimiter.toStdString().c_str(), preprocess);

    return result;
}

inline NetworkReport get_network_report_async(const NetworkMatrix &network)
{
    ReportRequest request = {};

    request.includeVertexStats = true;
    NetworkReport report = network.get_network_report(request);
    return report;
}

inline NetworkMatrix generate_network_async(const GeneratorParameters params)
{
    NetworkMatrix generated = NetworkGenerator::generate_network(params);

    return generated;
}

inline AlgorithmResult epidemic_simulation_async(const NetworkMatrix &network, const EpidemicParams &params)
{
    std::stringstream algLog;

    std::vector<EpidemicIterationInfo> result;

    azgra::Stopwatch stopwatch;

    stopwatch.start();
    switch (params.selectedModel)
    {
        case epidemic_impl::Model_SI:
        {
            result = SI_epidemic_model(network, params.initialInfected, params.infectionProb, params.iterationCount, algLog);
        }
        break;
        case epidemic_impl::Model_SIS:
        {
            result = SIS_epidemic_model(network, params.initialInfected, params.infectionProb, params.recoveryTime, params.iterationCount, algLog);
        }
        break;
        case epidemic_impl::Model_SIR:
        {
            result = SIR_epidemic_model(network, params.initialInfected, params.infectionProb, params.recoveryTime, params.iterationCount, algLog);
        }
        break;
    }
    stopwatch.stop();
    if (params.resultFileName.length() > 0)
    {
        save_cummulated_epidemic_stats(result, params.resultFileName.toStdString().c_str());
    }

    AlgorithmResult algResult(Alg_Epidemic);
    algResult.elapsedMilliseconds = stopwatch.elapsed_milliseconds();
    algResult.log = QString::fromStdString(algLog.str());

    return algResult;

}

inline AlgorithmResult girvan_newman_async(const NetworkMatrix &network, const uint maxIterCount, const float targetModularity,
                                           const QString &exportFileName)
{

    std::stringstream algLog;

    azgra::Stopwatch stopwatch;

    stopwatch.start();
    auto girvanNewmanResult = CommunityFinder::girvan_newman_divisive_clustering(network, algLog, maxIterCount, targetModularity);
    stopwatch.stop();

    if (exportFileName.length() > 0)
    {
        save_vertex_community_ids(exportFileName.toStdString().c_str(),
                                  CommunityFinder::get_vertex_community_ids(
                                      network.rows(),
                                      girvanNewmanResult[girvanNewmanResult.size() - 1]));
    }

    AlgorithmResult algResult(Alg_GirvanNewman);
    algResult.elapsedMilliseconds = stopwatch.elapsed_milliseconds();
    algResult.log = QString::fromStdString(algLog.str());
    algResult.communityEvolveResult = girvanNewmanResult;

    return algResult;
}


