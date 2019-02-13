from matrix import Mat


def load_bipartite():
    lines = []

#    with open("D:/gitrepos/dataAnalysisCourse/data/actorsAndMovies.txt") as dataset:
    with open("../data/actorsAndMovies.txt") as dataset:
        lines = dataset.readlines()

    labels = {}
    edges = []
    maxVertex = 0
    readingEdges = False
    for line in lines:
        line = line.strip()
        if line == "EDGES":
            readingEdges = True
            continue

        lineValues = line.split(";")
        if readingEdges:
            a = int(lineValues[0]) - 1
            b = int(lineValues[1]) - 1
            if a > maxVertex:
                maxVertex = a
            if b > maxVertex:
                maxVertex = b
            edges.append((a, b))
        else:
            labels[int(lineValues[0])] = lineValues[1].replace('"', "")

    rowCount = 10
    values = [[0 for c in range(len(labels))] for r in range(rowCount)]

    for edge in edges:
        (a, b) = edge
        values[a][b] = 1

    bm = Mat(values, tuple(labels.values()))
    return bm


def main():
    bipartiteMatrix = load_bipartite()
    # print(bipartiteMatrix)
    # bipartiteMatrix.print_labels()
    (leftMat, rightMat) = bipartiteMatrix.one_mode_projection_for_bipartite_graph()

    print("Matrix of actors. There is no connection.")
    leftMat.print_labels()
    print(leftMat)
    leftDistanceMat = leftMat.get_distance_matrix()
    #print(leftDistanceMat)
    # print(leftMat)
    # print("Average degree: " + str(leftMat.get_average_degree()))
    # print("Network average: " + str(leftMat.get_network_average(leftDistanceMat)))
    print(leftMat.get_degree_of_vertices())

    print("Matrix of movies")
    print(rightMat)

    print("Average degree: " + str(rightMat.get_average_degree()))
    print("Average clustering coefficient: " +
          str(rightMat.get_average_clustering_coefficients_for_vertices()))
    print("Edge count: " + str(rightMat.get_edge_count()))
    print(rightMat.get_degree_of_vertices())


if __name__ == "__main__":
    main()
