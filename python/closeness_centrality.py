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

    #print(edges)
    vertices = []
    [[vertices.append(x) for x in edge if x not in vertices] for edge in edges]

    matrixValues = [[is_edge_between(edges, v, rId) for v in range(1, len(vertices) + 1)] for rId in range(1, len(vertices) + 1)]
    matrix = Mat(matrixValues)
    
    matrix = matrix.set_inf_where_no_edge()
    distanceMatrix = matrix.floyd_distance()
    # print(matrix)
    closenessCentralityVector = distanceMatrix.closeness_centrality_for_vertices()
    print(closenessCentralityVector)

    # test = Mat([[0,3,5,'inf','inf'],['inf',0,1,2,'inf'],['inf','inf',0,'inf','inf'],['inf','inf', 9,0,1],[2,'inf',8,'inf',0]])
    # test.floyd_distance()
    # print(test)
    



    
    

if __name__ == "__main__":
    main()