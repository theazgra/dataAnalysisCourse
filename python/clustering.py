from matrix import Mat
from itertools import groupby
def is_edge_between(edges, a, b):
    if (a,b) in edges or (b,a) in edges:
        return 1
    else:
        return 0

def average_degree(degrees):
    average = sum(degrees[vertex] for vertex in degrees) / len(degrees)
    return average

def load_zachary_karate_club():
    edges = []
    with open("../data/KarateClub.csv", 'r') as csvFile:
        for row in csvFile:
            edges.append(tuple(map(int, row.strip().split(";"))))

    vertices = []
    [[vertices.append(x) for x in edge if x not in vertices] for edge in edges]

    matrixValues = [[is_edge_between(edges, v, rId) for v in range(1, len(vertices) + 1)] for rId in range(1, len(vertices) + 1)]
    matrix = Mat(matrixValues)
    return matrix

def main():
    matrix = load_zachary_karate_club()

    x = matrix.get_clustering_coefficients_for_vertices()
    print(x)
    for a in x:
       print("Vertex: {0}; Coefficient: {1}".format( a+1, x[a]))
    avg = sum(x.values()) / len(x)
    print("Average clustering coefficient in graph: {0}".format(avg))
    
    # print(matrix.get_degree_of_vertices())
    """ 
    1. Shlukovaci koeficient pro kazdy vrchol
    2. Prumerny shlukovaci koeficient cele site, + smerodatnou odchylku
    3. Distribuce shlukovaciho koeficientu vzhedem ke stupni
        - prumerny shlukovaci koeficient pro ruzne stupne vrcholu
    """
    matrix.save_clustering_coefficient_for_degree_groups("data.csv")



    # test = Mat([[0,3,5,'inf','inf'],['inf',0,1,2,'inf'],['inf','inf',0,'inf','inf'],['inf','inf', 9,0,1],[2,'inf',8,'inf',0]])
    # test.floyd_distance()
    # print(test)
    



    
    

if __name__ == "__main__":
    main()