from matrix import Mat
from random import choices
from math import log as ln
from math import inf

def get_probability_for_symmetric_network(vertexCount):
    result = ln(vertexCount) / float(vertexCount)
    return result

# P = ln(N) / N
def generate_random_network(vertexCount, edgeProbability):
    population  = [0, 1]
    weights     = [(1 - edgeProbability), edgeProbability] 

    matrixValues = [[inf for c in range(vertexCount)] for r in range(vertexCount)]
    

    for r in range(vertexCount):
        for c in range(r, vertexCount):
            if r == c:
                matrixValues[r][c] = 0
            elif matrixValues[r][c] == inf:
                edgeVal = choices(population, weights)[0]
                matrixValues[r][c] = edgeVal
                matrixValues[c][r] = edgeVal

    result = Mat(matrixValues)
    return result

def generate_small_world_network(startingSize, m, t):
    startingSize = 3
    resultSize = 3 + t
    currentSize = startingSize

    initial = Mat([[0,1,1],[1,0,1],[1,1,0]])
    vertexList = [0,0,1,1,2,2]
    resultMatrix = Mat([[0 for c in range(resultSize)] for r in range(resultSize)])
    resultMatrix.insert(initial)
    
    for step in range(t):
        newVertexIndex = startingSize + step
        vertexListSize = float(len(vertexList))
        population = range(currentSize)
        weights = [(float(vertexList.count(v)) / vertexListSize) for v in population]
        
        neighbours = []
        for mStep in range(m):
            neighbour = choices(population, weights)[0]
            while neighbour in neighbours:
                neighbour = choices(population, weights)[0]
            neighbours.append(neighbour)
        
        assert len(neighbours) == m
        
        vertexList.append(newVertexIndex)
        vertexList.append(newVertexIndex)
        for n in neighbours:
            vertexList.append(n)
            resultMatrix.values[newVertexIndex][n] = 1
            resultMatrix.values[n][newVertexIndex] = 1
            
        currentSize += 1
        #print("Current size: " + str(currentSize))

    return resultMatrix