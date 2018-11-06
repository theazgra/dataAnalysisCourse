from vec import Vec

class Cluster(object):
    def __init__(self, centroid):
        self.centroid = centroid
        self.objects = []

    def __eq__(self, other):
        if not isinstance(other, Cluster):
            return False
        else:
            return self.centroid == other.centroid

    def get_sse(self):
        sse = sum([(vec.euclidean_distance(self.centroid)**2) for vec in self.objects])
        return sse

    def vector_sum(self, vectors):
        result = Vec([0 for x in range(len(vectors[0].values))])
        for vector in vectors:
            result += vector
        return result

    def recalculate_centroid(self):
        if len(self.objects) == 0:
            raise Exception("Empty cluster")
        self.centroid = self.vector_sum(self.objects) / len(self.objects)
        return self.centroid

    def add_object(self, obj):
        self.objects.append(obj)

    def clear_objects(self):
        self.objects.clear()

    def __str__(self):
        return "Centroid: {0};SSE: {1}; {2} objects.".format(
            str(self.centroid), self.get_sse(), len(self.objects))
