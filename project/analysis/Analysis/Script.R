library(igraph)
library(ggplot2)
library(wesanderson)
library(GGally)

inputFile = "../../../data/ask-math/sx-mathoverflow-a2q.csv";

df = as.data.frame(read.csv(inputFile, header = F, sep = ";"));
g = graph_from_data_frame(df, directed = T);

edgeCount = length(E(g, directed = T,));

vertexCount = length(V(g));
vertexCount

edgeDensity = edge_density(g);
gReciprocity = reciprocity(g);

allDegree = degree(g, v = V(g), mode = "all")
inDegree = degree(g, mode = "in");
outDegree = degree(g, v = V(g), mode = "out");

length(allDegree)

allDegree
inDegree
outDegree

sum(allDegree == 1815)


min(inDegree);
max(inDegree);
mean(inDegree);

min(outDegree);
max(outDegree);
mean(outDegree);

min(allDegree);
max(allDegree);
mean(allDegree);

 inDegreeRel = degree_distribution(g, mode = "in");
outDegreeRel = degree_distribution(g, mode = "out");
allDegreeRel = degree_distribution(g, mode = "all");

inDegCumulative = degree_distribution(g, cumulative = T, mode = "in");
outDegCumulative = degree_distribution(g, cumulative = T, mode = "out");
allDegCumulative = degree_distribution(g, cumulative = T, mode = "all");

# Out degree distribution
ggplot() + geom_point(aes(c(0:(length(outDegreeRel) - 1)), outDegreeRel)) +
    scale_x_continuous("Výstupní stupeò", trans = "log10") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Relativní distribuce výstupních stupòù") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

# In degree distribution
ggplot() + geom_point(aes(c(0:(length(inDegreeRel) - 1)), inDegreeRel)) +
    scale_x_continuous("Vstupní stupeò", trans = "log10") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Relativní distribuce vstupních stupòù") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

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
closenessCen = closeness(g, mode = "total", normalized = T);


summary(inClosenessCentrality)

sum(inClosenessCentrality > 3.0e-09)




outClosenessCentrality = closeness(g, mode = "out", normalized = T);
allClosenessCentrality = closeness(g, mode = "all", normalized = T);

betweennessCentrality = betweenness(g, directed = T, normalized = F);
ggplot() + geom_point(aes(as.numeric(V(g)), y = as.numeric(betweennessCentrality))) +
    scale_x_continuous("Vrchol") +
    scale_y_continuous("betweenness centralita",trans = "log10") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

# Hubs have the most outgoing edges.
hs = hub_score(g)$vector
head(sort(hs,decreasing = T),5)


ggplot() + geom_point(aes(as.numeric(V(g)), y = as.numeric(hs))) +
    scale_x_continuous("Vrchol") +
    scale_y_continuous("Hodnota centrality") +
    ggtitle("Kleinbergova hub centralita") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));


# Authorities should have the most incoming edges.
authorities = authority_score(g)$vector;
ggplot() + geom_point(aes(as.numeric(V(g)), y = as.numeric(authorities))) +
    scale_x_continuous("Vrchol") +
    scale_y_continuous("Hodnota autority") +
    ggtitle("Kleinbergova autorita") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

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

map <- function(x, range = c(0, 1), from.range = NA) {
    if (any(is.na(from.range))) from.range <- range(x, na.rm = TRUE)

    ## check if all values are the same
    if (!diff(from.range)) return(
        matrix(mean(range), ncol = ncol(x), nrow = nrow(x),
            dimnames = dimnames(x)))

    ## map to [0,1]
    x <- (x - from.range[1])
    x <- x / diff(from.range)
    ## handle single values
    if (diff(from.range) == 0) x <- 0

    ## map from [0,1] to [range]
    if (range[1] > range[2]) x <- 1 - x
    x <- x * (abs(diff(range))) + min(range)

    x[x < min(range) | x > max(range)] <- NA

    x
}
