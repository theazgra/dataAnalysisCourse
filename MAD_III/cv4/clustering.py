import sklearn.cluster
import sklearn.metrics
import scipy.spatial
import numpy as np
import pandas as pd
import matplotlib.pyplot as plot


def main():
    fName = "data/clusters3.csv"
    data = np.loadtxt(fName, delimiter=';')

    # ac = sklearn.cluster.AgglomerativeClustering(n_clusters=4, affinity='euclid', linkage='complete').fit(data)

    # km = sklearn.cluster.KMeans(n_clusters=4).fit(data)

    # distMat = scipy.spatial.distance_matrix(data, data)
    # nnDist = np.where(distMat == 0, 99, distMat).min(axis=0)
    # seriesPlot = pd.Series(nnDist).hist(bins=40)
#     plot.show(seriesPlot)

    db = sklearn.cluster.DBSCAN(eps=0.8, min_samples=3).fit(data)

    # pAc = plot.scatter(data[:, 0], data[:, 1], c=ac.labels_)
    # pKm = plot.scatter(data[:, 0], data[:, 1], c=km.labels_)
    pDb = plot.scatter(data[:, 0], data[:, 1], c=db.labels_)

    # plot.show(pAc)
    # plot.show(pKm)
    plot.show(pDb)


if __name__ == "__main__":
    main()
