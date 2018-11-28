from matrix import Mat
import networkGenerator as ng

def is_edge_between(edges, a, b):
    if (a,b) in edges or (b,a) in edges:
        return 1
    else:
        return 0

def karate_club_average_degree():
    edges = []
    with open("../data/KarateClub.csv", 'r') as csvFile:
        for row in csvFile:
            edges.append(tuple(map(int, row.strip().split(";"))))

    print(edges)
    vertices = []
    [[vertices.append(x) for x in edge if x not in vertices] for edge in edges]

    matrixValues = [[is_edge_between(edges, v, rId) for v in range(1, len(vertices) + 1)] for rId in range(1, len(vertices) + 1)]
    matrix = Mat(matrixValues)
    

    degrees = matrix.get_degree_of_vertices()
    averageDegree = matrix.get_average_degree()
    print(degrees)
    print(averageDegree)

def generate_graph_and_its_properties():
    vertexCount = 1000
    probability = ng.get_probability_for_symmetric_network(vertexCount)
    print("Vertex count: {}".format(vertexCount))
    print("Edge probability: {}".format(probability))
    print("Expected average degree: {}".format((vertexCount - 1)*probability))
    network = ng.generate_random_network(vertexCount, probability)
    
    print("generated random network.")
    
    avgDegree = network.get_average_degree()
    avgClusteringCoeff  = network.get_average_clustering_coefficients_for_vertices()

    # print("Starting floyd.")
    # floydDistance = network.get_floyd_distance_matrix()
    # print("Ending floyd.")
    # print(floydDistance)
    # print("Clustering coefficient for vertices: {}".format(network.get_clustering_coefficients_for_vertices()))
    print("Average vertex degree: {}".format(avgDegree))
    print("Average clustering coefficient: {}".format(avgClusteringCoeff))
    print("Edge count: {}".format(network.get_edge_count()))

    network.export_network("../results/genNetwork.csv")

def small_world_experiment():
    sw = ng.generate_small_world_network(3, 2, 1000)
    sw.export_network("../results/sw.csv")


def main():
    generate_graph_and_its_properties()
    #small_world_experiment()
    #karate_club_average_degree()
    


    
    

if __name__ == "__main__":
    main()