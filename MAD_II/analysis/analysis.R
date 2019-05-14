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

gSimp <- simplify(g)
sir(gSimp, 0.2, 3, 10)

avgDist <- average.path.length(g, directed = F)
diam <- diameter(g, directed = F)

deg <- degree(g, mode = "all")
meanDeg <- mean(deg)
minDeg <- min(deg)
maxDeg <- max(deg)
length(deg[deg == minDeg])
length(deg[deg == maxDeg])



degNoLoops <- degree(g, mode = "all", loops = F)
meanDegdegNoLoops <- mean(degNoLoops)
minDegdegNoLoops <- min(degNoLoops)
maxDegdegNoLoops <- max(degNoLoops)

dd <- degree_distribution(g)
ddC <- degree_distribution(g, cumulative = T, mode="all")


ddDf <- data.frame(d = c(0:(length(dd) - 1)), freq = dd)
ggplot(ddDf, aes(x = d, y = freq)) + geom_point() +
    scale_x_continuous("Stupeň", trans = "log10") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Relativní distribuce stupňů") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

ddDfC <- data.frame(d = c(0:max(degree(g,mode="all"))), freq = 1-ddC)
ggplot(ddDfC, aes(x = d, y = freq)) + geom_line() +
    scale_x_continuous("Stupeň") +
    scale_y_continuous("Kumulativní frekvence") +
    ggtitle("Kumulativní distribuce stupňů") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

#c(0:(length(ddC) - 1))
#, trans = "log10"


ecc = eccentricity(g);
length(ecc[ecc == 0])

mean(ecc)
eccDf = data.frame(v = c(0:(length(ecc) - 1)), e = as.numeric(ecc));

ggplot(eccDf, aes(x = v, y = e)) +
    geom_point() + # geom_bar(stat = "identity") +
labs(x = "Vrchol", y = "Excentricita", title = "Excentricita vrcholů") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

cc <- closeness(g)
max(cc)
mean(cc)
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
mean(bc)
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

transitivity(g, type = "global", isolates = "zero");

trans <- transitivity(g, type = "local", isolates = "zero");
mean(trans)
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

cmp <- components(g)
cmp$no
max(cmp$csize)
min(cmp$csize)
mean(cmp$csize)
# Sireni jevu
# SiR model musime udelat sami a vysledky tady naimportoval....
#       gSimple <- simplify(g)
#       sirResult <- sir(gSimple, beta = 0.15, gamma = 2, no.sim = 100)

# Utok a Failure
simulationFailureAndAttack(g, 1000)

# Sampling
rnsFile <- "../../data/project/rns_sample_25p.csv"
resFile <- "../../data/project/res_sample_25p.csv"

rnsDf <- as.data.frame(read.csv(rnsFile, header = F, sep = ";"))
resDf <- as.data.frame(read.csv(resFile, header = F, sep = ";"))
gRns <- graph_from_data_frame(rnsDf, directed = F)
gRes <- graph_from_data_frame(resDf, directed = F)


length(V(gRns))
length(V(gRes))
length(E(gRns, directed = F))
length(E(gRes, directed = F))

mean(degree(gRns))
mean(degree(gRes))

average.path.length(gRns, directed = F)
average.path.length(gRes, directed = F)

diameter(gRns, directed = F)
diameter(gRes, directed = F)

transitivity(gRns, type = "global", isolates = "zero")
transitivity(gRes, type = "global", isolates = "zero")

mean(closeness(gRns))
mean(closeness(gRes))

mean(eccentricity(gRns))
mean(eccentricity(gRes))

mean(betweenness(gRns, directed = F, normalized = T))
mean(betweenness(gRes, directed = F, normalized = T))


ddRns <- degree_distribution(gRns)
ddRes <- degree_distribution(gRes)

ddRnsDf <- data.frame(d = c(0:(length(ddRns) - 1)), freq = ddRns)
ddResDf <- data.frame(d = c(0:(length(ddRes) - 1)), freq = ddRes)

pom = layout(mat = matrix(1:2, 1, 2, byrow = F));
layout.show()

plot(ddRnsDf, log = "xy",ylab="Relativní distrubuce stupňů",xlab="Stupeň",main="Random Node Sampling")
plot(ddResDf, log = "xy", ylab = "Relativní distrubuce stupňů", xlab = "Stupeň", main = "Random Edge Sampling")

ggplot(ddRnsDf, aes(x = d, y = freq)) + geom_point() +
    scale_x_continuous("Stupeň", trans = "log10") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Relativní distribuce stupňů") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

ggplot(ddResDf, aes(x = d, y = freq)) + geom_point() +
    scale_x_continuous("Stupeň", trans = "log10") +
    scale_y_continuous("Frekvence", trans = "log10") +
    ggtitle("Relativní distribuce stupňů") +
    theme(plot.title = element_text(hjust = 0.5, size = 30),
    axis.title = element_text(size = 22),
    axis.text = element_text(size = 18));

# Komunity

# Girvan-Newman algorithm
eb <- cluster_edge_betweenness(g, directed = F)
length(eb$modularity)
length(table(eb$membership)) # 433
barplot(table(eb$membership), xlab = "Id komunity", ylab = "Velikost komunity", log = "y", main = "Girvan-Newman")
modularity(eb) # [1] 0.8491724
is_hierarchical(eb)

mean(table(eb$membership))

save(file = "cluster_edge_betweenness.rbin", eb);

pom = layout(mat = matrix(1:4, 2, 2, byrow = F));
layout.show()

pom = layout(mat = matrix(1:3, 1, 3, byrow = T));
layout.show(pom);    

# Tries to find communities in graphs via directly optimizing a modularity score.
fg <- cluster_fast_greedy(g)
length(fg)
modularity(fg)
fg$algorithm

barplot(table(eb$membership), xlab = "Id komunity", ylab = "Velikost komunity",ylim=c(1,1000), log = "y", main = "Girvan-Newman")
# log = "y"
barplot(table(fg$membership), xlab = "Id komunity", ylab = "Velikost komunity",ylim=c(1,1000),log="y",main="Fast-Greedy")
is_hierarchical(fg)

louvain <- cluster_louvain(g)
length(louvain)
modularity(louvain)
barplot(table(louvain$membership), xlab = "Id komunity", ylab = "Velikost komunity", ylim = c(1, 1000), log = "y", main = "Louvain")
is_hierarchical(louvain)

walkCommunity <- cluster_walktrap(g, steps = 5)
barplot(table(walkCommunity$membership), xlab = "Id komunity", ylab = "Velikost komunity", ylim=c(1,1000), log="y", main = "Walktrap")
length(walkCommunity)
modularity(walkCommunity)



