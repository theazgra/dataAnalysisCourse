#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
#pragma once

#include <networkLib/network_matrix.h>
#include <azgra/io/text_file_functions.h>
#include <azgra/matrix.h>

template<typename T = double>
struct Point
{
    T x;
    T y;

    Point() = default;

    Point(T _x, T _y) : x(_x), y(_y)
    {}
};

template<typename T = double>
constexpr double euclidean_distance(const Point<double> &a, const Point<double> &b)
{
    return sqrt(pow((a.x - b.x), 2) + pow((a.y - b.y), 2));
}

azgra::networkLib::ClusterCandidate find_clusters_to_combine(const azgra::Matrix<double> &distanceMat,
                                                             const std::map<uint, azgra::networkLib::Cluster> &clusters)
{
    azgra::networkLib::ClusterCandidate candidate = {};
    candidate.distance = 9999999.99;

    for (const auto &cI : clusters)
    {
        for (const auto &cJ : clusters)
        {
            if (cI.first == cJ.first || cI.second.isDeleted || cJ.second.isDeleted)
                continue;
            double currDistance = distanceMat.at(cI.first, cJ.first);
            if (currDistance < candidate.distance)
            {
                candidate.set_new_dist(cI.second, cJ.second, currDistance);
            }
        }
    }

    assert(candidate.distance != 9999999.99);
    return candidate;
}

void delete_cluster(std::map<uint, azgra::networkLib::Cluster> &clusters, const uint key)
{
    size_t removedCount = clusters.erase(key);
    assert(removedCount == 1);
}

double get_average_linkage_distance(const std::vector<Point<double>> &points,
                                    const std::vector<uint> &aVertices,
                                    const std::vector<uint> &bVertices)
{
    always_assert(aVertices.size() <= points.size() && bVertices.size() <= points.size());

    double totalCount = static_cast<double>(aVertices.size() * bVertices.size());
    double dist = 0.0;

    for (const uint &u : aVertices)
    {
        for (const uint &v : bVertices)
        {
            dist += euclidean_distance(points[u], points[v]);
        }
    }
    dist /= totalCount;
    return dist;
}

double get_single_linkage_distance(const std::vector<Point<double>> &points,
                                   const std::vector<uint> &aVertices,
                                   const std::vector<uint> &bVertices)
{
    always_assert(aVertices.size() <= points.size() && bVertices.size() <= points.size());
    // Single linkage means the closest vertices, so distance must be minimal.
    double dist = std::numeric_limits<double>::max();

    for (const uint &u : aVertices)
    {
        for (const uint &v : bVertices)
        {
            double currDist = euclidean_distance(points[u], points[v]);
            if (currDist < dist)
            {
                dist = currDist;
            }
        }
    }
    return dist;
}


double get_complete_linkage_distance(const std::vector<Point<double>> &points,
                                     const std::vector<uint> &aVertices,
                                     const std::vector<uint> &bVertices)
{
    always_assert(aVertices.size() <= points.size() && bVertices.size() <= points.size());
    // Complete linkage means the furthest vertices, so distance must be maximal.
    double dist = std::numeric_limits<double>::min();

    for (const uint &u : aVertices)
    {
        for (const uint &v : bVertices)
        {
            double currDist = euclidean_distance(points[u],points[v]);
            if(currDist > dist)
            {
                dist=currDist;
            }
        }
    }
    return dist;
}

void recalculate_distance_matrix(const std::vector<Point<double>> &points,
                                 azgra::Matrix<double> &clusterMat,
                                 const std::map<uint, azgra::networkLib::Cluster> &clusters,
                                 const azgra::networkLib::Cluster &newCluster,
                                 const azgra::networkLib::LinkageType linkage)
{
    // We have to recalculate distance for each cluster according to newCluster.
    for (const auto &c : clusters)
    {
        // We don't recalculate similarity to same cluster.
        if (c.first == newCluster.representative)
            continue;

        double distance = 0.0f;

        switch (linkage)
        {
            case azgra::networkLib::LinkageType::LinkageType_Single:
                distance = get_single_linkage_distance(points, c.second.vertices, newCluster.vertices);
                break;
            case azgra::networkLib::LinkageType::LinkageType_Complete:
                distance = get_complete_linkage_distance(points, c.second.vertices, newCluster.vertices);
                break;
            case azgra::networkLib::LinkageType::LinkageType_Average:
                distance = get_average_linkage_distance(points, c.second.vertices, newCluster.vertices);
                break;

            default:
                assert(false);
                break;
        }

        clusterMat.at(c.first, newCluster.representative) = distance;
        clusterMat.at(newCluster.representative, c.first) = distance;
    }
}

std::vector<azgra::networkLib::Cluster>
find_clusters_hierarchical(const azgra::Matrix<double> &distanceMatrix,
                           const std::vector<Point<double>> &points,
                           const uint clusterCount,
                           azgra::networkLib::LinkageType linkType)
{
    always_assert(points.size() == distanceMatrix.rows());
    //const NetworkMatrix cosineMat = get_cosine_similarity_matrix(); replaced by distance matrix.
    //NetworkMatrix clusterMat = NetworkMatrix(cosineMat);
    azgra::Matrix<double> clusterMat = azgra::Matrix<double>(distanceMatrix);

    std::map<uint, azgra::networkLib::Cluster> clusters;

    // At start every point is in cluster of its own.
    for (uint vertex = 0; vertex < points.size(); vertex++)
        clusters[vertex] = azgra::networkLib::Cluster(vertex);

    uint currentClusterCount = clusters.size();

    // Until we have required cluster count...
    while (currentClusterCount > clusterCount)
    {
        // Fint two closest clusters.

        azgra::networkLib::ClusterCandidate candidate = find_clusters_to_combine(clusterMat, clusters);

        // Merge the two closest clusters.
        size_t beforeDelete = clusters.size(); // get_cluster_count(clusters);
        delete_cluster(clusters, candidate.i.representative);
        delete_cluster(clusters, candidate.j.representative);
        size_t afterDelete = clusters.size(); //get_cluster_count(clusters);

        assert(afterDelete == beforeDelete - 2);

        azgra::networkLib::Cluster newCluster(candidate.i.vertices, candidate.j.vertices);

        clusters[newCluster.representative] = newCluster;
        currentClusterCount = clusters.size();

        // Recalculate the cosineMat.
        recalculate_distance_matrix(points,clusterMat, clusters,newCluster,linkType);
    }
    std::vector<azgra::networkLib::Cluster> result(clusters.size());
    {
        size_t index = 0;
        for (const auto &c : clusters)
        {
            //result.push_back(c.second);
            result[index++] = c.second;
        }
    }
    printf("We are done with hierarchical clustering. Final cluster count: %lu\n", result.size());
    return result;
}

void tutorial_2()
{
    size_t reqClusterCount = 5;
    const char *dataFile = "/mnt/d/codes/git/dataAnalysisCourse/MAD_III/data/clusters5.csv";
    std::function<Point<double>(const azgra::string::SmartStringView<char> &)> fn = [](const azgra::string::SmartStringView<char> &line)
    {
        auto splitted = line.split(";");
        always_assert(splitted.size() == 2);
        return Point<double>(atof(splitted[0].data()), atof(splitted[1].data())); // NOLINT(cert-err34-c)
    };
    std::vector<Point<double>> points = azgra::io::parse_by_lines(dataFile, fn);
    fprintf(stdout, "Read %lu points\n", points.size());
    size_t pointCount = points.size();

    azgra::Matrix<double> distanceMatrix(pointCount, pointCount);

    for (size_t pointX = 0; pointX < pointCount; ++pointX)
    {
        for (size_t pointY = pointX + 1; pointY < pointCount; ++pointY)
        {
            double dist = euclidean_distance(points[pointX], points[pointY]);
            distanceMatrix.at(pointX, pointY) = dist;
            distanceMatrix.at(pointY, pointX) = dist;
        }
    }
    fprintf(stdout, "Calculated all distances\n");


    {
        auto clusters = find_clusters_hierarchical(distanceMatrix,
                                                   points,
                                                   reqClusterCount,
                                                   azgra::networkLib::LinkageType::LinkageType_Complete);

        int id = 1;
        for (const azgra::networkLib::Cluster &c : clusters)
        {
            printf("Cluster %i, vertex count: %lu\nVertices: ", id++, c.vertices.size());
            for (const uint &v : c.vertices)
                printf("%u, ", v);

            printf("\n");
        }

        // Output clustering result to csv file.
        {
            id=1;
            std::ofstream csvStream("clusteringResult.csv", std::ios::out);
            always_assert(csvStream.is_open());
            for (const azgra::networkLib::Cluster &c : clusters)
            {
                for (const uint &v : c.vertices)
                {
                    const Point p = points[v];
                    csvStream << p.x  << ';' << p.y << ';' << id << std::endl;
                }
                ++id;
            }
        }
    }
}

#pragma clang diagnostic pop