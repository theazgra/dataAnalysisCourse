from matrix import Mat

def is_edge_between(edges, a, b):
    if (a,b) in edges or (b,a) in edges:
        return 1
    else:
        return 0

def average_degree(degrees):
    average = sum(degrees[vertex] for vertex in degrees) / len(degrees)
    return average

def main():
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
    averageDegree = average_degree(degrees)
    print(degrees)
    print(averageDegree)
    


    
    

if __name__ == "__main__":
    main()