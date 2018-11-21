from itertools import groupby
from collections import OrderedDict

class Mat(object):
    def __init__(self, values):
        self.rowCount = len(values)
        self.colCount = len(values[0])

        for row in values:
            if (len(row)) != self.colCount:
                raise Exception(
                    "Bad matrix values. Values need to have same number of elements in every column.")

        self.values = list(list(map(self._tryToFloat, row)) for row in values)

        """
        @values will be enumerable with some enumerables in it.
        """
    def set_all_values(self, value):
        self.values =  tuple(tuple(value for cId in range(0, self.colCount)) for rId in range(0, self.rowCount))

    def col(self, colIndex):
        return tuple(row[colIndex] for row in self.values)

    def row(self, rowIndex):
        return self.values[rowIndex]

    def _tryToFloat(self, val):
        try:
            floatVal = float(val)
            return floatVal
        except ValueError:
            return val

    def __str__(self):
        result = ""
        for row in self.values:
            result += ("\t" + str(row) + "\n")
        return result

    def _checkSameDims(self, other):
        if not isinstance(other, Mat):
            raise Exception("Other is not of type Mat.")

        if (self.rowCount != other.rowCount) or (self.colCount != other.colCount):
            raise Exception("Different matrix dimensions.")

    def __add__(self, other):
        self._checkSameDims(other)
        newValues = [[(self.values[rid][cid] + other.values[rid][cid])
                      for cid in range(0, self.colCount)] for rid in range(0, self.rowCount)]
        return Mat(newValues)

    def __sub__(self, other):
        self._checkSameDims(other)
        newValues = [[(self.values[rid][cid] - other.values[rid][cid])
                      for cid in range(0, self.colCount)] for rid in range(0, self.rowCount)]
        return Mat(newValues)
    
    def transpose(self):
        """
        Fix for matrices, which aren't squared.
        """
        newValues = tuple(tuple(self.values[cId][rId] for cId in range(0, self.colCount)) for rId in range(0, self.rowCount))
        return Mat(newValues)
                
    def diagonal(self):
        return [self.values[rId][rId] for rId in range(0, self.rowCount)]
    
    def get_degree_of_vertices(self):
        result = {}
        for vertex in range(0, self.rowCount):
            degree = sum(self.row(vertex))
            result[vertex] = degree
        return result

    def get_average_degree(self):
        degrees = self.get_degree_of_vertices()
        average = sum(degrees[vertex] for vertex in degrees) / len(degrees)
        return average

    def _vector_dot_product(self, vecA, vecB):
        if len(vecA) != len(vecB):
            raise Exception("Vector sizes don't match.")
        return sum((vecA[index] * vecB[index]) for index in range(0, len(vecA)))

    def _matrix_mul(self, other):
        if self.colCount != other.rowCount:
            raise Exception("These matrices can not be multiplied together.")

        return Mat([[self._vector_dot_product(self.row(rId), other.col(cId)) for cId in range(0, self.colCount)] for rId in range(0, self.rowCount)])

    def __mul__(self, other):
        if isinstance(other, int) or isinstance(other, float):
            newValues = [[(self.values[rid][cid] * other)
                          for cid in range(0, self.colCount)] for rid in range(0, self.rowCount)]
            return Mat(newValues)
        if isinstance(other, Mat):
            return self._matrix_mul(other)
    
    
    def set_inf_where_no_edge(self):
        return Mat(tuple(tuple((self.values[rId][cId] if (self.values[rId][cId] > 0) else float('inf')) for cId in range(0, self.colCount)) for rId in range(0, self.rowCount)))

    def get_floyd_distance_matrix(self):
        tmpMat = Mat(self.values)
        dimRange = range(0, self.colCount)
        for k in dimRange:
            for i in dimRange:
                for j in dimRange:
                    tmp = tmpMat[i][k] + tmpMat[k][j]
                    if tmpMat[i][j] > tmp:
                        tmpMat[i][j] = tmp

        return self
    
    def closeness_centrality_for_vertices(self):
        vertexCount = self.rowCount
        result = []

        for rId in range(0, self.rowCount):
            result.append(round(1 / (sum([self.values[rId][cId] for cId in range(0, self.colCount) if cId != rId]) / vertexCount), 7))

        return result

    def get_neighbours(self, vertex):
        row = self.values[vertex]
        neighbours = [ nId for nId in range(0, self.colCount) if row[nId] == 1 and nId != vertex]
        return neighbours
    
    def get_edge_count_between_neighbours(self, neighbours):
        result = 0
        for n1 in neighbours:
            for n2 in neighbours:
                if n1 != n2 and self.values[n1][n2] == 1:
                    result += 1
        return (result / 2)

    def get_average_clustering_coefficients_for_vertices(self):
        clusteringCoeffForVertices = self.get_clustering_coefficients_for_vertices()
        result = sum(clusteringCoeffForVertices.values()) / len(clusteringCoeffForVertices)
        return result

    def get_clustering_coefficients_for_vertices(self, vertices=None):
        if vertices == None:
            vertices = range(0, self.rowCount)
        result = {}
        for vertex in vertices:
            vertexNeighbours = self.get_neighbours(vertex)
            neighbourCount = len(vertexNeighbours)
            if neighbourCount > 1:
                edgesBetweenNeighbours = self.get_edge_count_between_neighbours(vertexNeighbours)
                clusteringCoefficien = (2.0 * edgesBetweenNeighbours) / (neighbourCount * (neighbourCount - 1))
                result[vertex] = clusteringCoefficien    
            else:
                result[vertex] = 0
            
        return result

    
    def get_vertices_grouped_by_degree(self):
        degreesDict = self.get_degree_of_vertices()
        result = {}
        for vertex in degreesDict:
            if degreesDict[vertex] not in result:
                result[degreesDict[vertex]] = []
            result[degreesDict[vertex]].append(vertex)
        return result

    def save_clustering_coefficient_for_degree_groups(self, resultFilename):
        groups = self.get_vertices_grouped_by_degree()
        result = {}
        for groupDegree in groups:
            groupClusteringCoeffs = self.get_clustering_coefficients_for_vertices(groups[groupDegree])
            groupSize = len(groups[groupDegree])
            summedClusteringCoefficient = sum(groupClusteringCoeffs.values())
            groupAvg = summedClusteringCoefficient / float(groupSize)
            result[groupDegree] = groupAvg
        
        result = OrderedDict(sorted(result.items(), key=lambda t: t[0]))
        # result = sorted(result)

        with open(resultFilename, 'w', encoding='utf8') as of:
            for degree in result:
                of.write("{0};{1}\n".format(degree, result[degree]))
        return result

    def export_network(self, filename):
        with open(filename, 'w') as file:
            for r in range(self.rowCount):
                for c in range(r, self.colCount):
                    if r != c and self.values[r][c] == 1:
                        file.write("{0};{1}\n".format(r,c))