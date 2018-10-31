from math import sqrt

class Vec(object):
    def __init__(self, values):
        self.values = tuple([self._tryToFloat(v) for v in values])
        self.size = len(self.values)
    
    def _tryToFloat(self, val):
        try:
            v = float(val)
            return v
        except ValueError:
            return val

    def __iter__(self):
        return iter(self.values)

    def __getitem__(self, key):
        return self.values[key]

    def euclidean_norm(self):
        return sqrt(sum([v**2 for v in self.values]))
    
    def euclidean_distance(self, other):
        self._check(other)
        return sqrt(sum([((self.values[index] - other.values[index])**2) for index in range(0, self.size)]))

    def at(self, index):
        if index >= self.size:
            raise ValueError("Index out of vector bounds.")
        return self.values[index]
    
    def dotProduct(self, other):
        self._check(other)
        return self.__mul__(other)
    
    def _check(self, other, checkInstance=True):
        if checkInstance and not isinstance(other, Vec):
            raise Exception("Other is not of type Vec.")
        if self.size != other.size:
            raise ValueError("Vectors are of different sizes.")

    def __lt__(self, other):
        self._check(other)
        return self.euclidean_norm().__lt__(other.euclidean_norm())

    def __le__(self, other):
        self._check(other)
        return self.euclidean_norm().__le__(other.euclidean_norm())
        
    def __gt__(self, other):
        self._check(other)
        return self.euclidean_norm().__gt__(other.euclidean_norm())
        
    def __ge__(self, other):        
        self._check(other)
        return self.euclidean_norm().__ge__(other.euclidean_norm())

    def __eq__(self, other):
        if isinstance(other, Vec):
            return self.values ==  other.values
        return False

    def __add__(self, other):
        self._check(other)
        newValues = [(self.values[index] + other.values[index]) for index in range(0, self.size)]
        return Vec(newValues)

    def __radd__(self, other):
        return self.__add__(other)

    def __sub__(self, other):
        self._check(other)
        newValues = [(self.values[index] - other.values[index]) for index in range(0, self.size)]
        return Vec(newValues)

    def __rsub__(self, other):
        return self.__sub__(other)

    def __mul__(self, other):
        # Multiplication by scalar
        if isinstance(other, float) or isinstance(other, int):
            newValues = [(self.values[index] * other) for index in range(0, self.size)]
            return Vec(newValues)
        
        # Dot product
        if isinstance(other, Vec):
            dotProduct = sum([(self.values[index] * other.values[index]) for index in range(0, self.size)])
            return dotProduct

    def __rmud__(self, other):
        return self.__mul__(other)

    def __str__(self):
        return str(self.values)
    