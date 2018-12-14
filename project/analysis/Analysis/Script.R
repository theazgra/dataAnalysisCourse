library(igraph)
library(ggplot2)
library(wesanderson)
library(GGally)


inputFile = "../../../data/ask-math/sx-mathoverflow-a2q.csv";
df = as.data.frame(read.csv(inputFile, header = F, sep = ";"));
g = graph_from_data_frame(df, directed = T);
edgeCount = length(E(g, directed = T,));
vertexCount = length(V(g));
vertices = as.numeric(V(g)$name)

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

indd = degree_distribution(g, mode = "in");
inddDf = data.frame(d = c(0:(length(indd) - 1)), freq = indd)
ggplot(inddDf, aes(x = d, y = freq)) + geom_point() +
    scale_x_continuous("Stupeò", trans = "log10") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Relativní distribuce vstupních stupòù") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

ggplot(inddDf, aes(y = freq)) + geom_bar() +
    scale_x_continuous("Stupeò", trans = "log10") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Relativní distribuce vstupních stupòù") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

outdd = degree_distribution(g, mode = "out");
outddDf = data.frame(d = c(0:(length(outdd) - 1)), freq = outdd)

ggplot(outddDf, aes(x = d, y = freq)) + geom_point() +
    scale_x_continuous("Stupeò", trans = "log10") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Relativní distribuce výstupních stupòù") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

alldd = degree_distribution(g, mode = "all");
allddDf = data.frame(d = c(0:(length(alldd) - 1)), freq = alldd);
# All degree distribution
ggplot(allddDf, aes(x = d, y = freq)) + geom_point() +
    scale_x_continuous("Stupeò", trans = "log10") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Relativní distribuce stupòù") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));


inDdCum = degree_distribution(g, cumulative = T, mode = "in");
inDdCumDf = data.frame(d = c(0:(length(inDdCum) - 1)), cumFreq = inDdCum);
ggplot(inDdCumDf, aes(x = d, y = cumFreq)) + geom_point() +
    scale_x_continuous("Stupeò") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Kumulativní distribuce vstupních stupòù") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

outDdCum = degree_distribution(g, cumulative = T, mode = "out");
outDdCumDf = data.frame(d = c(0:(length(outDdCum) - 1)), cumFreq = outDdCum);
ggplot(outDdCumDf, aes(x = d, y = cumFreq)) + geom_point() +
    scale_x_continuous("Stupeò") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Kumulativní distribuce výstupních stupòù") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

allDdCum = degree_distribution(g, cumulative = T, mode = "all");
allDdCumDf = data.frame(d = c(0:(length(allDdCum) - 1)), cumFreq = allDdCum);
ggplot(allDdCumDf, aes(x = d, y = cumFreq)) + geom_point() +
    scale_x_continuous("Stupeò") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Kumulativní distribuce stupòù") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));




# In closeness centrality cumulative
closenessCentr <- closeness(g, mode = "all", weights = NA)

closenessDf = data.frame(v = vertices, cc = as.numeric(closenessCentr))
closenessDf2 = data.frame(v = c(0:(length(closenessCentr)-1)), cc = as.numeric(closenessCentr))

ggplot(closenessDf2, aes(x = v, y = cc)) +
    geom_point() +
    scale_x_continuous("Vrchol") +
    scale_y_continuous("Closeness centralita") +
    ggtitle("Closeness centralita") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

bc = betweenness(g, directed = T, normalized = F);
bcDf = data.frame(v = vertices, bcVal = as.numeric(bc));
bcDf2 = data.frame(v = c(0:(length(bc) - 1)), bcVal = as.numeric(bc));



ggplot(bcDf2, aes(x = v, y = bcVal)) + geom_point() +
    scale_x_continuous("Vrchol") +
    scale_y_continuous("Betweenness centralita",trans = "log10") +
    ggtitle("Betweenness centralita") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));


degDf = data.frame(degree = as.numeric(names(deg)), freq = as.numeric(deg));

deg = table(degree(g, mode = "all"))

ggplot(bcDf, aes(x = degree, y = freq)) +
    geom_bar(stat = "identity", width = 0.1) +
    scale_x_continuous("degree", trans = "log10") +
    scale_y_continuous("frequency")

# Hubs have the most outgoing edges.
hs = hub_score(g)$vector
hsDf = data.frame(v = vertices, h = as.numeric(hs));
hsDf2 = data.frame(v = c(0:(length(hs)-1)), h = as.numeric(hs));


ggplot(hsDf2, aes(x = v, y = h)) + geom_point() +
    scale_x_continuous("Vrchol") +
    scale_y_continuous("Hodnota centrality", trans = "log10") +
    ggtitle("Kleinbergova hub centralita") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));


# Authorities should have the most incoming edges.
authorities = authority_score(g)$vector;
authoritiesDf = data.frame(v = c(0:(length(authorities) - 1)), a = as.numeric(authorities));

ggplot(authoritiesDf, aes(x = v, y = a)) +
    geom_point() +
    scale_x_continuous("Vrchol") +
    scale_y_continuous("Hodnota autority", trans = "log10") +
    ggtitle("Kleinbergova autorit centralita") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

sum(authoritiesDf$a > 1e-8)

globalTrans = transitivity(g, type = "global",isolates = "zero");
localTrans = transitivity(g, type = "local", isolates = "zero");

sum(localTrans == 0);
sum(localTrans == 1);


mean(localTrans)
summary(localTrans)

transDf = data.frame(v = c(0:(length(localTrans) - 1)), t = localTrans)
ggplot(transDf, aes(x = v, y = t)) +
    geom_point() +
    scale_x_continuous("Vrchol") +
    scale_y_continuous("Shlukovací koeficient") +
    ggtitle("Shlukovací koeficient pro vrcholy") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));


diam = diameter(g, directed = T);
directedGdirectedGraphDiameter = diameter(g, directed = T);

ecc = eccentricity(g, mode = "out");

sum(ecc != 0)
# IMPORTANT HERE

eccXRange = c(0 : (length(ecc) - 1))
eccDf = data.frame(v = eccXRange, e = as.numeric(ecc));

ggplot(eccDf, aes(x = v, y = e)) +
    geom_bar(stat = "identity") +
    labs(x = "Vrchol", y = "Excentricita", title = "Excentricita vrcholù") + 
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

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
