from math import sqrt, pi, exp, inf
from vec import Vec
from random import randint
from random import shuffle
from cluster import Cluster
from itertools import chain
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

def get_var_w_mean(dataset, mean):
    var = sum([(eucld(val, mean)**2) for val in dataset]) / float(len(dataset))
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

def ff(val):
    return str(val).replace('.', ',')


def find_cluster_by_centroid(clusters, centroid):
    for cluster in clusters:
        if cluster.centroid == centroid:
            return cluster
    raise Exception(
        ("Cluster with given centroid {0} wasn't found.".format(str(centroid))))


def k_means(dataset, k):
    originalCentroids = [dataset[randint(0, len(dataset)-1)] for x in range(k)]
    #originalCentroids = dataset[:k]
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


def do_k_means(df):
    try:
        k = int(input("K: "))
        print("K-means for k = " + str(k))
    except:
        raise Exception("Bad k")
    #k = 3
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

    for c in clusters:
        print(c)
    print("Total SSE: " + str(totalSse))

def do_normal_distribution(df):
    cols = 4
    for colIndex in range(cols):
        distribution = get_normal_distribution(df, colIndex)
        oF = "D:/gitrepos/dataAnalysisCourse/results/distribution_{}.csv".format(colIndex)
        with open(oF, 'w') as outFile:
            for x in distribution:
                outFile.write("{0};{1}\n".format(ff(x), ff(distribution[x])))

def do_cumulative_distribution(df):
    cols = 4
    for colIndex in range(cols):
        cumulativeDistribution = get_cumulative_distribution(df, colIndex)
        oF = "../results/cumulative_distribution_{}.csv".format(colIndex)
        with open(oF, 'w') as outFile:
            for x in cumulativeDistribution:
                outFile.write("{0};{1}\n".format(ff(x), ff(cumulativeDistribution[x])))

def do_mean_distance(df): 
    meanVector = Vec([0,0,0,0])
    for v in df:
        meanVector += v
    meanVector /= len(df)        
    distances = [v.euclidean_distance(meanVector) for v in df]
    
    distanceMean = sum(distances) / len(distances)
    distanceVar = get_var_w_mean(distances, distanceMean)
    print(distanceMean)
    print(distanceVar)
    
    distanceNormalDistribution = {}
    for d in distances:
        distanceNormalDistribution[d] = normal_distribution_point(d, distanceMean, distanceVar)

    oF = "D:/gitrepos/dataAnalysisCourse/results/distance_normal_distribution.csv"
    with open(oF, 'w') as outFile:
        for x in distanceNormalDistribution:
            outFile.write("{0};{1}\n".format(ff(x), ff(distanceNormalDistribution[x])))
    print("Done")

def create_fold(df, k, do_shuffle=True):
    if do_shuffle:
        shuffle(df)
    
    result = []
    partSize = len(df) / k
    for kIndex in range(k):
        result.append(df[int(kIndex * partSize) : int((kIndex * partSize) + partSize)])

    return result


def k_NN(df, k, x, classIndex):
    for v in df:
        v.customDistance = x.euclidean_distance(v, 4)
    
    distanceSorted = sorted(df, key=lambda vector: vector.customDistance)
    
    nn = distanceSorted[0 : k]
    classDict = {}
    
    for v in nn:
        if v[classIndex] not in classDict:
            classDict[v[classIndex]] = 0
        classDict[v[classIndex]] += 1
    
    maxClass = max(classDict, key=lambda kv: classDict[kv])
    return maxClass
            

def k_NN_for_df(training, k, testing, classIndex):
    classes = []
    correct = 0
    for x in testing:
        guessedClass = k_NN(training, k, x, classIndex)
        if guessedClass == x[classIndex]:
            correct += 1
        else: 
            classes.append("c: {0};g: {1}".format(x[classIndex], guessedClass))
    
    correctness = round(correct / len(testing),4)
    return [correctness, classes]

def do_kNN(df):
    foldSize = 10
    k = 4
    classIndex = 4
    kFold = create_fold(df, foldSize)
    #result = k_NN_for_df(kFold[1], k, kFold[0], classIndex)

    for foldIndex in range(foldSize):
        testing = kFold[foldIndex]
        training = kFold.copy()
        training.pop(foldIndex)
        training = list(chain.from_iterable(training))

        foldResult = k_NN_for_df(training, k, testing, classIndex)
        print(foldResult)


def main():
    df = load_iris_dataset_wo_species("D:/gitrepos/dataAnalysisCourse/data/iris.csv")
    dfAll = load_iris_dataset("D:/gitrepos/dataAnalysisCourse/data/iris.csv")

    do_kNN(dfAll)
    
    #do_k_means(df)
    #result = get_frequencies_of_attributes(df)
    #do_normal_distribution(df)
    #do_cumulative_distribution(df)
    #do_mean_distance(df)

if __name__ == "__main__":
    main()
