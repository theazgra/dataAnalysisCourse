from math import sqrt, pi, exp, inf
from vec import Vec
from random import randint
from cluster import Cluster
from scipy.integrate import quad
from sys import argv


def load_iris_dataset(file):
    with open(file, 'r') as irisFile:
        dataset = [Vec(line.strip().split(',')) for line in irisFile]
        return dataset[1:]


def load_iris_dataset_wo_species(file):
    with open(file, 'r') as irisFile:
        dataset = [Vec(line.strip().split(',')[:-1]) for line in irisFile]
        return dataset[1:]


def eucld(a, b):
    return (a - b)
    # return sqrt((a - b)**2)


def get_mean(dataset, col):
    return sum([r[col] for r in dataset]) / float(len(dataset))


def get_var(dataset, col):
    mean = get_mean(dataset, col)
    var = sum([(eucld(r[col], mean)**2)
               for r in dataset]) / float(len(dataset))
    return var


def normal_distribution_point(x, mean, var):
    result = (1.0 / sqrt(2*pi*var)) * exp(-1*(((x-mean)**2)/(2*var)))
    return result


def get_normal_distribution(dataset, col):
    mean = get_mean(dataset, col)
    var = get_var(dataset, col)
    result = {}
    for vector in dataset:
        x = vector[col]
        result[x] = normal_distribution_point(x, mean, var)
    return result


def get_probability(dataset, col, val):
    valCount = len([v for v in dataset if v[col] == val])
    result = (valCount / len(dataset) )
    return result


def get_cumulative_distribution(dataset, col):
    result = {}
    for vector in dataset:
        x = vector[col]
        values = remove_duplicates([v[col] for v in dataset if v[col] <= x])
        probability = sum([get_probability(dataset, col, v) for v in values])
        result[x] = probability

    return result

    """
    pro kazde X, najit x ktere jsou <= X, potom pro kazde (x zjistit pocet vyskytu a podelit poctem zaznamu) 
    a secist to
    """


def ff(val):
    return str(val).replace('.', ',')


def find_cluster_by_centroid(clusters, centroid):
    for cluster in clusters:
        if cluster.centroid == centroid:
            return cluster
    raise Exception(
        ("Cluster with given centroid {0} wasn't found.".format(str(centroid))))


def k_means(dataset, k):
    #originalCentroids = [dataset[randint(0, len(dataset)-1)] for x in range(k)]
    originalCentroids = dataset[:k]
    clusters = [Cluster(centroid) for centroid in originalCentroids]
    newCentroids = []
    iteration = 0

    while True:
        iteration += 1

        # Clear each cluster before start.
        for cluster in clusters:
            cluster.clear_objects()

        # Assign each vector to cluster based on which centroid is closest.
        for vector in dataset:
            closestCentroid = None
            minDistance = inf
            for centroid in originalCentroids:
                dist = vector.euclidean_distance(centroid)
                if dist < minDistance:
                    minDistance = dist
                    closestCentroid = centroid

            cluster = find_cluster_by_centroid(clusters, closestCentroid)
            cluster.add_object(vector)

        # Recalculate centroids inside clusters.
        for cluster in clusters:
            cluster.recalculate_centroid()

        newCentroids = [cluster.centroid for cluster in clusters]
        if newCentroids == originalCentroids:
            print("Finished k-mean after {} iterations.".format(iteration))
            break
        else:
            originalCentroids = newCentroids

    return clusters


def remove_duplicates(iterable):
    l = list(set(iterable))
    l.sort()
    return l


def get_frequencies_of_attributes(dataset):
    cols = dataset[0].size
    attr = []
    vecCount = len(dataset)
    for c in range(cols):
        attrFreq = {}
        values = remove_duplicates([v[c] for v in dataset])
        for val in values:
            absFreq = len([v for v in dataset if v[c] == val])
            relFreq = absFreq / vecCount
            cummFreq = absFreq + sum([v[0] for v in attrFreq.values()])
            attrFreq[val] = [absFreq, relFreq, cummFreq]

        attr.append(attrFreq)

    return attr


def main():
    cols = 4
    # DEBUG
    #df = load_iris_dataset_wo_species("data/iris.csv")
    df = load_iris_dataset_wo_species("../data/iris.csv")

    # List of attributes
    #   Each list has dictionary of unique value - [absolute frequence, relative frequence, commulative frequency]
    #result = get_frequencies_of_attributes(df)
    # https://homel.vsb.cz/~kud007/lectures/madi_07.pdf
    # 16

    # try:
    #     k = int(input("K: "))
    #     print("K-means for k = " + str(k))
    # except:
    #     raise Exception("Bad k")
    k = 3
    clusters = k_means(df, k)
    totalSse = sum([c.get_sse() for c in clusters])

    indexes = []
    
    for v in df:
        index = 0
        for c in clusters:
            if v in c.objects:
                indexes.append(index)
                break
            index += 1
    
    print(indexes)
    # for c in clusters:
    #     print(c)
    # print("Total SSE: " + str(totalSse))

    # normal distribution
    # for colIndex in range(cols):
    #     distribution = get_normal_distribution(df, colIndex)
    #     oF = "../results/distribution_{}.csv".format(colIndex)
    #     with open(oF, 'w') as outFile:
    #         for x in distribution:
    #             outFile.write("{0};{1}\n".format(ff(x), ff(distribution[x])))

    # cumulative distribution
    # for colIndex in range(cols):
    #     cumulativeDistribution = get_cumulative_distribution(df, colIndex)
    #     oF = "../results/cumulative_distribution_{}.csv".format(colIndex)
    #     with open(oF, 'w') as outFile:
    #         for x in cumulativeDistribution:
    #             outFile.write("{0};{1}\n".format(ff(x), ff(cumulativeDistribution[x])))


if __name__ == "__main__":
    main()
