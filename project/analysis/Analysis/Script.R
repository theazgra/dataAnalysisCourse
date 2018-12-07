library(igraph)
library(ggplot2)
library(GGally)

inputFile = "../../../data/ask-ubuntu/sx-askubuntu-a2q.csv";

df = as.data.frame(read.csv(inputFile, header = F, sep = ";"));
g = graph_from_data_frame(df, directed = T);

edgeCount = length(E(g, directed = T));
vertexCount = length(V(g));

edgeDensity = edge_density(g);
gReciprocity = reciprocity(g);

allDegree = degree(g, mode = "all")
inDegree = degree(g, mode = "in");
outDegree = degree(g, mode = "out");

inMinDeg = min(inDegree);
inMaxDeg = max(inDegree);
inMeanDeg = mean(inDegree);

outMinDeg = min(outDegree);
outMaxDeg = max(outDegree);
outMeanDeg = mean(outDegree);

allMinDeg = min(allDegree);
allMaxDeg = max(allDegree);
allMeanDeg = mean(allDegree);

inDegreeRel = degree_distribution(g, mode = "in");
outDegreeRel = degree_distribution(g, mode = "out");
allDegreeRel = degree_distribution(g, mode = "all");

inDegCumulative = degree_distribution(g, cumulative = T, mode = "in");
outDegCumulative = degree_distribution(g, cumulative = T, mode = "out");
allDegCumulative = degree_distribution(g, cumulative = T, mode = "all");

# Out degree distribution
ggplot() + geom_point(aes(c(0:(length(outDegreeRel) - 1)), outDegreeRel)) +
    scale_x_continuous("Out degree", trans = "log10") +
    scale_y_continuous("Frequency", trans = "log10");

# In degree distribution
ggplot() + geom_point(aes(c(0:(length(inDegreeRel) - 1)), inDegreeRel)) +
    scale_x_continuous("In degree", trans = "log10") +
    scale_y_continuous("Frequency", trans = "log10");

# All degree distribution
ggplot() + geom_point(aes(c(0:(length(allDegreeRel) - 1)), allDegreeRel)) +
    scale_x_continuous("Degree", trans = "log10") +
    scale_y_continuous("Frequency", trans = "log10");


# In degree cumulative
ggplot() + geom_point(aes(0:max(inDegree), (1 - inDegCumulative))) +
    scale_x_continuous("In degree", trans = "log10") +
    scale_y_continuous("Cumulative frequency");

# Out degree cumulative
ggplot() + geom_point(aes(0:max(outDegree), (1 - outDegCumulative))) +
    scale_x_continuous("Out degree", trans = "log10") +
    scale_y_continuous("Cumulative frequency");

# All degree cumulative
ggplot() + geom_point(aes(0:max(allDegree), (1 - allDegCumulative))) +
    scale_x_continuous("Degree", trans = "log10") +
    scale_y_continuous("Cumulative frequency");

# In closeness centrality cumulative
inClosenessCentrality = closeness(g, mode = "in", normalized = T);
outClosenessCentrality = closeness(g, mode = "out", normalized = T);
allClosenessCentrality = closeness(g, mode = "all", normalized = T);

betweennessCentrality = betweenness(g, directed = T);

# Hubs have the most outgoing edges.
hubs = hub_score(g)$vector;
bestHubs = head(sort(hubs, decreasing = T), 5);
bestHubs

head(sort(outDegree, decreasing = T), 5)

# Authorities should have the most incoming edges.
authorities = authority_score(g)$vector;
bestAuthorities = head(sort(authorities, decreasing = T), 5);
bestAuthorities

head(sort(inDegree, decreasing = T), 5)

trans = transitivity(g, type = "global");

directedGdirectedGraphDiameter = diameter(g, directed = T);

meanDistance = mean_distance(g, directed = T);
allDistances = distances(g, weights = NA, mode = "in");

#complete subgraphs of an undirected graph
completeSubgraphs = cliques(g);

completeSubgraphsSizes = sapply(completeSubgraphs, length);

clusters = cluster_edge_betweenness(g);
dendPlot(ceb, mode = "hclust")

summary(g)


# http://sachaepskamp.com/files/Cookbook.html
# http://statmath.wu.ac.at/research/friday/resources_WS0708_SS08/igraph.pdf