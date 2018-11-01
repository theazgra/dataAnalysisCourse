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
        return "Cluster;Centroid: {0} and {1} objects.".format(
            str(self.centroid), len(self.objects))
