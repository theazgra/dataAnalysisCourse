# https://snap.stanford.edu/data/ca-GrQc.html

# install.packages("testit")


library(igraph)
library(ggplot2)
library(testit)
source("failure_attack_simulation.R")

setwd("D:/gitrepos/dataAnalysisCourse/MAD_II/analysis")
collabFile <- "../../data/project/gr_qc_collab.csv"

collabDf <- as.data.frame(read.csv(collabFile, header = F, sep = ""))
g <- graph_from_data_frame(collabDf, directed = F)

g <- simplify(g, remove.loops = F)
vc <- length(V(g))
ec <- length(E(g))

avgDist <- average.path.length(g, directed = F)
diam <- diameter(g, directed = F)

deg <- degree(g, mode = "all")
meanDeg <- mean(deg)
minDeg <- min(deg)
maxDeg <- max(deg)

degNoLoops <- degree(g, mode = "all", loops = F)
meanDegdegNoLoops <- mean(degNoLoops)
minDegdegNoLoops <- min(degNoLoops)
maxDegdegNoLoops <- max(degNoLoops)

dd <- degree_distribution(g)
ddC <- degree_distribution(g, cumulative = T)


ddDf <- data.frame(d = c(0:(length(dd) - 1)), freq = dd)
ggplot(ddDf, aes(x = d, y = freq)) + geom_point() +
    scale_x_continuous("Stupeň", trans = "log10") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Relativní distribuce stupňů") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

ddDfC <- data.frame(d = c(0:(length(ddC) - 1)), freq = ddC)
ggplot(ddDfC, aes(x = d, y = freq)) + geom_point() +
    scale_x_continuous("Stupeň") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Kumulativní distribuce stupňů") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));


ecc = eccentricity(g);

eccDf = data.frame(v = c(0:(length(ecc) - 1)), e = as.numeric(ecc));

ggplot(eccDf, aes(x = v, y = e)) +
    geom_point() + # geom_bar(stat = "identity") +
labs(x = "Vrchol", y = "Excentricita", title = "Excentricita vrcholů") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

cc <- closeness(g)
ccDf <- data.frame(v = c(0:(length(cc) - 1)), cc = as.numeric(cc))

ggplot(ccDf, aes(x = v, y = cc)) +
    geom_point() +
    scale_x_continuous("Vrchol") +
    scale_y_continuous("Closeness centralita") +
    ggtitle("Closeness centralita") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

bc <- betweenness(g, directed = F)
bcDf <- data.frame(v = c(0:(length(bc) - 1)), bc = as.numeric(bc))
ggplot(bcDf, aes(x = v, y = bc)) +
    geom_point() +
    scale_x_continuous("Vrchol") +
    scale_y_continuous("Betweenness centralita", trans = "log10") +
    ggtitle("Betweenness centralita") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

hs <- hub_score(g)

hsDf <- data.frame(v = c(0:(length(hs$vector) - 1)), hubScore <- as.numeric(hs$vector))

ggplot(hsDf, aes(x = v, y = hubScore)) + geom_point() +
    scale_x_continuous("Vrchol") +
    scale_y_continuous("Hodnota centrality", trans = "log10") +
    ggtitle("Kleinbergova hub centralita") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));



trans <- transitivity(g, type = "local", isolates = "zero");
transDf <- data.frame(v = c(0:(length(trans) - 1)), t = trans)
trans

ggplot(transDf, aes(x = v, y = t)) +
    geom_point() +
    scale_x_continuous("Vrchol") +
    scale_y_continuous("Shlukovací koeficient") +
    ggtitle("Shlukovací koeficient pro vrcholy") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));



# Sireni jevu
# SiR model musime udelat sami a vysledky tady naimportoval....
#       gSimple <- simplify(g)
#       sirResult <- sir(gSimple, beta = 0.15, gamma = 2, no.sim = 100)

# Utok a Failure
simulationFailureAndAttack(g, 1000)

# Sampling
# Possibly not in R, we must create sample in our app and load it here...

# Komunity

# Girvan-Newman algorithm
eb <- cluster_edge_betweenness(g, directed = F)
length(eb$modularity)
length(table(eb$membership)) # 433
barplot(table(eb$membership), xlab="Id komunity",ylab="Velikost komunity",log="y")
modularity(eb) # [1] 0.8491724
is_hierarchical(eb) 
    

save(file = "cluster_edge_betweenness.rbin", eb);


# load(file = "cluster_edge_betweenness.rbin")
getwd()

# Tries to find communities in graphs via directly optimizing a modularity score.
fg <- cluster_fast_greedy(g)
length(fg)
modularity(fg)
fg$algorithm
barplot(table(fg$membership), xlab = "Id komunity", ylab = "Velikost komunity", log = "y")
is_hierarchical(fg)

louvain <- cluster_louvain(g)
length(louvain)
modularity(louvain)
barplot(table(louvain$membership), xlab = "Id komunity", ylab = "Velikost komunity", log = "y")
is_hierarchical(louvain)

walkCommunity <- cluster_walktrap(g, steps = 5)
barplot(table(walkCommunity$membership), xlab = "Id komunity", ylab = "Velikost komunity", log = "y")
length(walkCommunity)
modularity(walkCommunity)

ebDend <- as.dendrogram(eb);
plot(ebDend, cut(ebDend, h = 20)$upper)