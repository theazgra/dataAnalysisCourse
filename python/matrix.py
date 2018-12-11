from itertools import groupby
from collections import OrderedDict
from queue import Queue
from math import inf


class Mat(object):
    def __init__(self, values, labels=None):
        self.rowCount = len(values)
        self.colCount = len(values[0])
        self.labels = labels

        for row in values:
            if (len(row)) != self.colCount:
                raise Exception(
                    "Bad matrix values. Values need to have same number of elements in every column.")

        self.values = [[self._tryToFloat(rowVal)
                        for rowVal in row] for row in values]
        #self.values = list(list(map(self._tryToFloat, row)) for row in values)

    def get_label(self, vertex):
        assert vertex < self.rowCount
        if self.labels == None:
            return ""
        else:
            return self.labels[vertex]

    def print_labels(self):
        if self.labels != None:
            for r in range(self.rowCount):
                print("Vertex: {}: {}".format(r, self.labels[r]))

    def set_all_values(self, value):
        self.values = [[value for c in range(
            self.colCount)] for r in range(self.rowCount)]

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
        newValues = tuple(tuple(self.values[cId][rId] for cId in range(
            0, self.colCount)) for rId in range(0, self.rowCount))
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

    def get_network_average(self, distanceMatrix):
        max = 0
        for row in self.values:
            for rowVal in row:
                if rowVal > max and rowVal != inf:
                    max = rowVal
        return max

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

    def get_distance_matrix(self):
        distanceMat = Mat(self.values)
        distanceMat.set_all_values(inf)
        for row in range(self.rowCount):
            for col in range(row, self.colCount):
                dist = self.bfs(row, col)
                distanceMat.values[row][col] = dist
                distanceMat.values[col][row] = dist
        return distanceMat

    def get_floyd_distance_matrix(self):
        tmpMat = Mat(self.values)
        dimRange = range(0, self.colCount)
        for k in dimRange:
            for i in dimRange:
                for j in dimRange:
                    tmp = tmpMat.values[i][k] + tmpMat.values[k][j]
                    if tmpMat.values[i][j] > tmp:
                        tmpMat.values[i][j] = tmp

        return tmpMat

    def closeness_centrality_for_vertices(self):
        vertexCount = self.rowCount
        result = []

        for rId in range(0, self.rowCount):
            result.append(round(
                1 / (sum([self.values[rId][cId] for cId in range(0, self.colCount) if cId != rId]) / vertexCount), 7))

        return result

    def get_neighbours(self, vertex):
        row = self.values[vertex]
        neighbours = [nId for nId in range(
            0, self.colCount) if row[nId] == 1 and nId != vertex]
        return neighbours

    def get_edge_count(self):
        edgeCount = 0
        for r in range(self.rowCount):
            for c in range(r, self.colCount):
                if self.values[r][c] > 0 and self.values[r][c] != inf:
                    edgeCount += 1
        return edgeCount

    def get_edge_count_between_neighbours(self, neighbours):
        result = 0
        for n1 in neighbours:
            for n2 in neighbours:
                if n1 != n2 and self.values[n1][n2] == 1:
                    result += 1
        return (result / 2)

    def get_average_clustering_coefficients_for_vertices(self):
        clusteringCoeffForVertices = self.get_clustering_coefficients_for_vertices()
        result = sum(clusteringCoeffForVertices.values()) / \
            len(clusteringCoeffForVertices)
        return result

    def get_clustering_coefficients_for_vertices(self, vertices=None):
        if vertices == None:
            vertices = range(0, self.rowCount)
        result = {}
        for vertex in vertices:
            vertexNeighbours = self.get_neighbours(vertex)
            neighbourCount = len(vertexNeighbours)

            if neighbourCount > 1:
                edgesBetweenNeighbours = self.get_edge_count_between_neighbours(
                    vertexNeighbours)
                clusteringCoefficien = (
                    2.0 * edgesBetweenNeighbours) / (neighbourCount * (neighbourCount - 1))
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
            groupClusteringCoeffs = self.get_clustering_coefficients_for_vertices(
                groups[groupDegree])
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
                        file.write("{0};{1}\n".format(r, c))

    def insert(self, other):
        if self.colCount < other.colCount or self.rowCount < other.colCount:
            raise Exception("Cannot insert bigger matrix into smaller one.")

        for row in range(other.rowCount):
            for col in range(other.colCount):
                self.values[row][col] = other.values[row][col]

    def bfs(self, source, dest):
        visited = [False for c in range(self.colCount)]
        prev = [-1 for c in range(self.colCount)]
        current = None

        q = Queue()
        q.put(source)
        neib = []

        found = False
        while not q.empty():
            current = q.get()
            if current == dest:
                found = True
                break

            if current >= self.rowCount:
                visited[current] = True
                continue

            neib = self.get_neighbours(current)
            for n in neib:
                if not visited[n]:
                    q.put(n)
                    visited[n] = True
                    prev[n] = current

        if not found:
            return inf

        path = 0
        current = dest
        while current != source:
            path += 1
            current = prev[current]

        return path

    def one_mode_projection_for_bipartite_graph(self):
        # Left bipartite side
        leftMatValues = [[0 for c in range(self.rowCount)]
                         for r in range(self.rowCount)]
        rightMatValues = [[0 for c in range(self.colCount - self.rowCount)]
                          for r in range(self.colCount - self.rowCount)]

        for v1 in range(self.rowCount):
            for v2 in range(self.rowCount):
                if v1 == v2:
                    continue
                for col in range(self.colCount):
                    if self.values[v1][col] > 0 and self.values[v2][col] > 0:
                        leftMatValues[v1][v2] = 1
                        leftMatValues[v2][v1] = 1

        for v1 in range(self.colCount):
            for row in range(self.rowCount):
                for v2 in range(self.colCount):
                    if v1 == v2:
                        continue
                    if self.values[row][v1] > 0 and self.values[row][v2] > 0:
                        rightMatValues[v1 -
                                       self.rowCount][v2 - self.rowCount] = 1
                        rightMatValues[v2 -
                                       self.rowCount][v1 - self.rowCount] = 1

        leftMat = Mat(leftMatValues, self.labels)
        rightMat = Mat(rightMatValues)
        return (leftMat, rightMat)
