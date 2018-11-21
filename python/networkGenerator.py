from matrix import Mat
from random import choices
from math import log as ln
from math import inf

def get_probability_for_symmetric_network(vertexCount):
    result = ln(vertexCount) / float(vertexCount)
    return result

# P = ln(N) / N
def generate_network(vertexCount, edgeProbability):
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
