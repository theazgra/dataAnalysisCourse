import pandas as pd
import sklearn.cluster
import sklearn.metrics
import matplotlib.pyplot as plot
import scipy
import numpy as np


def agglomerative(dff):
    acResults = []
    for k in range(2, 100):
        ac = sklearn.cluster.AgglomerativeClustering(
            n_clusters=k, affinity='euclid', linkage='complete').fit(dff)

        ss = sklearn.metrics.silhouette_score(dff, ac.labels_)
        #print(f"n_clusters={k} silhouette_score={ss}")
        acResults.append({'k': k, 'ss': ss})

    print(acResults)
    bestSs = -1.0
    bestK = 0
    for x in acResults:
        if x['ss'] > bestSs and x['k'] > 2 and x['k'] < 20:
            bestSs = x['ss']
            bestK = x['k']

    print(f"Best k is: {bestK} with ss: {bestSs}")


def kmeans(dff):
    results = []
    for k in range(2, 10):
        km = sklearn.cluster.KMeans(n_clusters=k).fit(dff)
        sse = km.inertia_
        ss = sklearn.metrics.silhouette_score(dff, km.labels_)
        results.append({'k': k, 'sse': sse, 'ss': ss})
        print(f"n_clusters={k} silhouette_score={ss}")

    # nejlepe vychazi pro k = 4 pouze dle ss
    # n_clusters=4 silhouette_score=0.30200816770427846
    print(results)
    dfcs = pd.DataFrame.from_dict(results, orient='columns')
    dfcs = dfcs.set_index('k')
    ssePlot = dfcs.sse.plot()
    # ssPlot = dfcs.ss.plot()
    plot.show()
    # dle sse pak 5, ale ss po 4 rychle pada


def dbscan(dff):
    e = 0.5
    ms = 3
    # dist = scipy.spatial.distance_matrix(dff, dff)
    # distNN = np.where(dist == 0, 99, dist).min(axis=0)
    # pd.Series(distNN).hist(bins=40)
    # plot.show()
    # print(distNN)
    # db = sklearn.cluster.DBSCAN(eps=e, min_samples=ms).fit(dff)
    # ss = sklearn.metrics.silhouette_score(dff, db.labels_)
    # cc = len(set(db.labels_))
    # print(
    #     f"eps: {e}; min_samples: {ms}; ss: {ss}; label count: {cc}")
    for ie in range(1, 11):
        e = 0.5 + ie*0.01
        for i in range(2, 12):
            ms = i
            db = sklearn.cluster.DBSCAN(eps=e, min_samples=ms).fit(dff)
            ss = sklearn.metrics.silhouette_score(dff, db.labels_)
            cc = len(set(db.labels_))
            print(
                f"eps: {e}; min_samples: {ms}; ss: {ss}; label count: {cc}")


def create_for_visualization(df, labels):
    clusteringResult = []
    for i in range(0, len(df)):
        clusteringResult.append({
            'country': df[i]['Country'],
            'clusters': labels[i]
        })
    return clusteringResult


def main():
    df = pd.read_csv("happiness_report_2017.csv")
    # numerical
    dfn = df.drop(['Country', 'Happiness.Rank'], axis=1)
    # features
    dff = dfn.drop(
        ['Happiness.Score', 'Whisker.high', 'Whisker.low'], axis=1)

    countryList = df['Country'].tolist()
    print(dff.shape)

    # -1 mean that vertices are in wrong clusters, mixing
    # around 0 overlapping clusters
    # close to 1 higly separated clusters

    # agglomerative(dff)
    # acFinal = sklearn.cluster.AgglomerativeClustering(
    #     n_clusters=6, affinity='euclid', linkage='complete').fit(dff)

    # kmeans(dff)
    # dbscan(dff)

    # km4 = sklearn.cluster.KMeans(n_clusters=4).fit(dff).labels_
    km5 = sklearn.cluster.KMeans(n_clusters=5).fit(dff).labels_
    dff['Cluster'] = km5
    dffMean = dff.groupby(['Cluster']).mean()
    dffMin = dff.groupby(['Cluster']).min()
    dffMax = dff.groupby(['Cluster']).max()
    # print(dff)
    print(dffMean)
    # print(dffMin)
    # print(dffMax)

    result5 = []
    for i in range(0, 5):
        result5.append([])
        for ci in range(0, len(countryList)):
            if km5[ci] == i:
                result5[i].append(countryList[ci])

    [print(x) for x in result5]

    # result4 = []
    # for i in range(0, 4):
    #     result4.append([])
    #     for ci in range(0, len(countryList)):
    #         if km4[ci] == i:
    #             result4[i].append(countryList[ci])

    # [print(x) for x in result4]
    # print("-----------------------------------------------------------------------------")


if __name__ == "__main__":
    main()
