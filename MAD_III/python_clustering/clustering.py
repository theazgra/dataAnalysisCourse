import sklearn.cluster, sklearn.metrics
import scipy.spatial
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def display_clustering(data, labels):
    p = plt.scatter(data[:, 0], data[:, 1], c=labels)
    plt.show(p)


# clusters5n
# fileClusters5n = "../data/clusters5n.csv"
#     fileSinusoid = "../data/densegrid.csv"
#     X = np.loadtxt(fileClusters5n, delimiter=";")

#     distMat = scipy.spatial.distance_matrix(X, X)
#     closestDistances = np.where(distMat == 0, 99, distMat).min(axis=0)

#     print(closestDistances)
#     epsilon = np.average(closestDistances)
#     print(epsilon)
#     # h = pd.Series(closestDistances).hist(bins=40)
#     # plt.show(h)

#     # scores = []
#     # for sample in range(2, 11):
#     #     sampleResult = sklearn.cluster.DBSCAN(eps=epsilon, min_samples=sample).fit(X)
#     #     sil = sklearn.metrics.silhouette_score(X, sampleResult.labels_)
#     #     scores.append(
#     #         {
#     #             "epsilon": epsilon,
#     #             "min_samples": sample,
#     #             # "sse": sampleResult.inertia_,
#     #             "silhouette": sil,
#     #         }
#     #     )
#     #     display_clustering(X, sampleResult.labels_)
#     # for s in scores:
#     #     print(s)
#     kmeanScores = []
#     for k in range(2, 15):
#         kmean = sklearn.cluster.KMeans(n_clusters=k).fit(X)
#         kmeanScores.append(
#             {
#                 "k": k,
#                 "sse": kmean.inertia_,
#                 "silhouette": sklearn.metrics.silhouette_score(X, kmean.labels_),
#             }
#         )

#     for s in kmeanScores:
#         print(s)

#     df_clustering_scores = pd.DataFrame.from_dict(kmeanScores, orient="columns")
#     df_clustering_scores = df_clustering_scores.set_index("k")
#     plt.show(df_clustering_scores.sse.plot())
#     plt.show(df_clustering_scores.silhouette.plot())

#     k = sklearn.cluster.KMeans(n_clusters=8).fit(X)
#     display_clustering(X,k.labels_)


def main():
    fileClusters5n = "../data/clusters5n.csv"
    fileSinusoid = "../data/densegrid.csv"
    X = np.loadtxt(fileSinusoid, delimiter=";")

    distMat = scipy.spatial.distance_matrix(X, X)
    closestDistances = np.where(distMat == 0, 99, distMat).min(axis=0)

    # print(closestDistances)
    epsilon = np.average(closestDistances)
    print(epsilon)
    # h = pd.Series(closestDistances).hist(bins=40)
    # plt.show(h)


    dbs = sklearn.cluster.DBSCAN(eps=0.2, min_samples=12).fit(X)
    display_clustering(X,dbs.labels_)
    # return

    scores = []
    for sample in range(0, 50):
        teps = 0.05+(sample*0.01)
        print("testing eps: {}".format(teps))
        try:
            sampleResult = sklearn.cluster.DBSCAN(eps=teps, min_samples=12).fit(X)
            sil = sklearn.metrics.silhouette_score(X, sampleResult.labels_)
            print("Silhouette: {}".format(sil))
        except:
            print("Failed")
        finally:
            print("Failed")
        
        # scores.append({"epsilon": epsilon, "min_samples": sample, "silhouette": sil})
        # print('done {}'.format(sample))
        # display_clustering(X, sampleResult.labels_)
    # for s in scores:
    #     print(s)

    df_clustering_scores = pd.DataFrame.from_dict(scores, orient="columns")
    df_clustering_scores = df_clustering_scores.set_index("min_samples")
    plt.show(df_clustering_scores.silhouette.plot())

    # kmeanScores = []
    # for k in range(2, 15):
    #     kmean = sklearn.cluster.KMeans(n_clusters=k).fit(X)
    #     kmeanScores.append(
    #         {
    #             "k": k,
    #             "sse": kmean.inertia_,
    #             "silhouette": sklearn.metrics.silhouette_score(X, kmean.labels_),
    #         }
    #     )

    # for s in kmeanScores:
    #     print(s)

    # df_clustering_scores = pd.DataFrame.from_dict(kmeanScores, orient="columns")
    # df_clustering_scores = df_clustering_scores.set_index("k")
    # plt.show(df_clustering_scores.sse.plot())
    # plt.show(df_clustering_scores.silhouette.plot())

    # k = sklearn.cluster.KMeans(n_clusters=8).fit(X)
    # display_clustering(X,k.labels_)


if __name__ == "__main__":
    main()
