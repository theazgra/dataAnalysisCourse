from Table.Cell import CellData
from Table.PropertyCombination import AssociationRule
from Table.vec import Vec
import itertools
from math import sqrt

class CellData(object):
    def __init__(self, row, col, value):
        self.col = col
        self.row = row
        self.value = value

    def is_same_type(self, other):
        return self.value == other.value

    def match(self, otherCell):
        return self.value == otherCell.value

    def __str__(self):
        return "Row: {0};Col: {1};Value: {2}".format(self.row, self.col, self.value)

class DataTable(object):
    def __init__(self, csvFile, resultIndex=None, positiveValue=None, delimiter=','):
        self.header = []
        self.data = []
        self.resultIndex = resultIndex
        self.colCount = 0
        self.positiveValue = positiveValue
        self.associationRules = None

        row = -1
        with open(csvFile, 'r') as csv:
            writeHeader = True
            for line in csv:
                row += 1
                if writeHeader:
                    self.header = line.strip().split(delimiter)
                    self.colCount = len(self.header)
                    writeHeader = False
                else:
                    values = line.strip().split(delimiter)
                    col = -1
                    line = []
                    for v in values:
                        col += 1
                        line.append(CellData(row, col, v))
                    self.data.append(line)

        self.rowCount = len(self.data)
        if resultIndex != None:
            self.positiveValueCount = sum(
                1 for x in self.data if x[self.resultIndex].value == self.positiveValue)
            self.negativeValueCount = self.rowCount - self.positiveValueCount

    def print_data(self):
        [print(col) for col in self.data]

    def get_result_col(self):
        return [col[self.resultIndex] for col in self.data]

    def get_values_in_col(self, colIndex):
        values = []
        for row in self.data:
            if row[colIndex] not in values:
                values.append(row[colIndex])
        return values

    def print_value_statistics(self):
        for colIndex in range(0, self.colCount):
            print("Column {0} has:\t{1} different values.".format(
                self.header[colIndex], len(self.get_values_in_col(colIndex))))

    def _add_combination(self, combinations, newCombination):
        d = {}
        for c in newCombination:
            if c.col not in d:
                d[c.col] = 1
            else:
                return

        combinations.append(AssociationRule(newCombination))

    def generate_all_combinations(self):
        differentValues = []

        for row in self.data:
            for colIndex in range(0, len(self.header)):
                if colIndex == self.resultIndex:
                    continue
                val = row[colIndex]
                if val.value not in [cell.value for cell in differentValues]:
                    differentValues.append(val)

        allCombs = []
        for combinationSize in range(1, self.colCount):
            combs = list(itertools.combinations(
                differentValues, combinationSize))
            for c in combs:
                self._add_combination(allCombs, c)

        self._check_combinations(allCombs)
        self.associationRules = allCombs
        return allCombs

    def order_rules_by(self, lambdaFunction, desc):
        self.associationRules.sort(key=lambdaFunction, reverse=desc)

    def _check_combinations(self, rules):
        for rule in rules:
            for row in self.data:
                if rule.match(row[0: len(row) - 1]):
                    rule.matchedRows += 1

                    rowResult = row[len(row) - 1].value
                    if rowResult == self.positiveValue:
                        rule.positiveMatchCount += 1
                    else:
                        rule.negativeMatchCount += 1
            rule.support = rule.matchedRows / self.rowCount
            rule.confidence = (rule.positiveMatchCount /
                               rule.matchedRows) if rule.matchedRows > 0 else 0

    def row(self, index, columns=None):
        if index >= self.rowCount:
            raise ValueError("Row index out of bounds.")
        if columns != None and len(columns) > 0:
            return Vec([cell.value for cell in self.data[index] if cell.col in columns])
        else:
            return Vec([cell.value for cell in self.data[index]])

    def col(self, index):
        if index >= self.colCount:
            raise ValueError("Column index out of bounds.")
        return Vec([row[index].value for row in self.data])

    def calculate_mean_vector(self, columns):
        columnValues = {}

        for row in self.data:
            for col in columns:
                if col not in columnValues:
                    columnValues[col] = 0.0
                columnValues[col] += float(row[col].value)

        result = Vec([(columnValues[col] / self.rowCount)
                      for col in columnValues])
        return result

    def calculate_total_variance(self, meanVector, cols):
        if meanVector.size != len(cols):
            raise ValueError("Size mismatch between mean vector and specified")

        result = sum([((self.row(rowIndex, cols).euclidean_distance(meanVector))**2)
                      for rowIndex in range(0, self.rowCount)]) / self.rowCount

        return result
