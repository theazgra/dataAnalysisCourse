if (!require("igraph")) 
  install.packages("igraph")
library(igraph)

swDf <- read.csv("../../results/sw.csv", header = F, sep = ";")
swGraph <- graph_from_data_frame(swDf, directed = F)
swDeg <- degree(swGraph, mode = "all")
plot(swGraph, vertex.size = swDeg)

 generated <- read.csv("../../results/genNetwork.csv", header = F, sep = ";")
generatedGraph <- graph_from_data_frame(generated, directed = F)
plot(generatedGraph)
plot(generatedGraph, layout = layout_randomly)
plot(generatedGraph, layout = layout_in_circle)
plot(generatedGraph, layout = layout_on_sphere)
plot(generatedGraph, layout = layout_with_fr)
plot(generatedGraph, layout = layout_as_tree)




df <- read.csv("../../data/KarateClub.csv", header = F, sep = ";", dec = ",")
df

g1 <- graph_from_data_frame(d = df, directed = F)
class(g1) # urceni "typu" datoveho objektu 
E(g1)
V(g1)
plot(g1)

as_edgelist(g1, names = F) # muzeme mit take seznam hran
A <- as_adjacency_matrix(g1, names = FALSE) # nebo matici sousednosti
print(A)

plot()
plot(g1, layout = layout_randomly)
plot(g1, layout = layout_in_circle)
plot(g1, layout = layout_on_sphere)
plot(g1, layout = layout_with_fr)

deg <- degree(g1, mode="all")
plot(g1, vertex.size=deg * 2)
print(deg)
deg["34"] 
min(deg)
max(deg)
mean(deg) # prumerny stupen
hist(deg, breaks = 1:vcount(g1)-1, main = "Histogram stupnu", xlab = "stupen", ylab = "cetnost")

barplot(table(deg))

# pomoci ggplot2
library(ggplot2)
t.deg <- table(deg) # dostaneme cetnosti
df2 <- data.frame(degree = as.numeric(names(t.deg)),freq = as.numeric(t.deg)) # ggplot chce data.frame
ggplot(df2,aes(x = degree, y = freq)) + geom_bar(stat = "identity", width = 0.4) + scale_x_continuous(breaks=scales::pretty_breaks(n = 17)) + scale_y_continuous(breaks=scales::pretty_breaks(n = 12)) 

# relativni cetnosti
deg_rel <- degree_distribution(g1, mode = "all")
# vykreslime
ggplot()+geom_point(aes(c(0:(length(deg_rel)-1)),deg_rel))+ scale_x_continuous(breaks=c(0:17)) + scale_y_continuous(breaks=scales::pretty_breaks(n = 12)) + labs(x = "degree", y = "freq.") + theme_bw()
# pokud nechceme stupen == 0
ggplot()+geom_point(aes(c(1:(length(deg_rel)-1)),deg_rel[-1])) + scale_x_continuous(breaks=c(1:17)) + scale_y_continuous(breaks=scales::pretty_breaks(n = 12)) + labs(x = "degree", y = "freq.") + theme_bw()

# kumulativni distribuce 
deg.dist <- degree_distribution(g1, cumulative=T, mode="all")
plot(x=0:max(deg), y=1-deg.dist, pch=19, cex=1.2, col="orange", xlab="Degree", ylab="Cumulative Frequency")

# kumulativni distribuce, ale pomoci CCDF, Complementary cumulative distribution function, P(X > x)!!!!! 
deg.dist.CCDF <- 1 - degree_distribution(g1, cumulative=T, mode="all")
plot(x=0:max(deg), y=1-deg.dist.CCDF, pch=19, cex=1.2, col="orange", xlab="Degree", ylab="Cumulative Frequency")

