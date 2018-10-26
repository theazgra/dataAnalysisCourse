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