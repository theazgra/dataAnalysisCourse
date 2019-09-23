#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
#pragma once

#include <networkLib/network_matrix.h>
#include <azgra/io/text_file_functions.h>

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

void tutorial_2()
{
    size_t reqClusterCount = 3;
    const char *dataFile = "/mnt/d/codes/git/dataAnalysisCourse/MAD_III/data/clusters3.csv";
    //size_t reqClusterCount = 5;
    //const char *dataFile = "/mnt/d/codes/git/dataAnalysisCourse/MAD_III/data/clusters5n.csv";
    std::function<Point<double>(const azgra::string::SmartStringView<char> &)> fn = [](const azgra::string::SmartStringView<char> &line)
    {
        auto splitted = line.split(";");
        always_assert(splitted.size() == 2);
        return Point<double>(atof(splitted[0].data()), atof(splitted[1].data())); // NOLINT(cert-err34-c)
    };
    std::vector<Point<double>> points = azgra::io::parse_by_lines(dataFile, fn);
    fprintf(stdout, "Read %lu points\n", points.size());
    size_t pointCount = points.size();

    azgra::networkLib::NetworkMatrix mat(pointCount, pointCount);

//#pragma omp parallel for
    for (size_t pointX = 0; pointX < pointCount; ++pointX)
    {
        for (size_t pointY = pointX + 1; pointY < pointCount; ++pointY)
        {
            double dist = euclidean_distance(points[pointX], points[pointY]);
            mat.at(pointX, pointY) = static_cast<float>( dist);
            mat.at(pointY, pointX) = static_cast<float>( dist);
        }
    }
    fprintf(stdout, "Calculated all distances\n");

    mat.hierarchical_clustering(reqClusterCount, "clustering.txt", azgra::networkLib::LinkageType::LinkageType_Single, false);
}

#pragma clang diagnostic pop