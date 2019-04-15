library(igraph)
library(lsa)

g = make_graph("Zachary") # nacti Karate club
coords = layout_with_fr(g) # jaka je to lazoutivaci metoda?
plot(g, layout=coords, vertex.label=NA, vertex.size=10)

############################
# greedy method (hiearchical, fast method)
c1 = cluster_fast_greedy(g)

membership(c1)
length(c1)
sizes(c1)
crossing(c1, g)
plot(c1, g, layout=coords) 
plot(g, vertex.color=membership(c1), layout=coords) # vyexportujte
plot_dendrogram(c1) # vyexportujte
modularity(c1)
############################
# Sami pro dalsi metody
c2 = cluster_leading_eigen(g) # jaka je to metoda ?
 
############################
# Sami 
c3 = cluster_edge_betweenness(g) # jaka je to metoda ?
modularity(c3)
############################
# Sami 
c4 = cluster_optimal(g) # jaka je to metoda ?
modularity(c4)
############################
# hierarchicke shlukovani

S=similarity(g, method = "invlogweighted") # co to je za metodu ("invlogweighted")?
D = 1-S

# vyzkousejte i dalsi dve metody pro vypocet podobnosti
 
# distance object
d = as.dist(D)
 
# average-linkage clustering method
cc = hclust(d, method = "average")
 
# plot dendrogram
plot(cc)
 
# draw blue borders around clusters
clusters.list = rect.hclust(cc, k = 4, border="blue") # riznete i pro k = 3, k = 2

# cut dendrogram at 4 clusters
clusters = cutree(cc, k = 4)

plot(g, vertex.color=clusters, layout=coords)

#######################################

# global similarity
I = diag(1, vcount(g));

# leading eigenvalue
l = eigen(A)$values[1]

# 85% of leading eigenvalue
alpha = 0.85 * 1/l

# similarity matrix
S = solve(I - alpha * A)

# largest value
max = max(as.vector(S))

# distance matrix
D = max-S

# set null distance from a node to itself
d = diag(D)
D = D + diag(-d, vcount(g))


# distance object
d = as.dist(D)

# average-linkage clustering method
cc = hclust(d, method = "average")

# plot dendrogram
plot(cc)

# draw blue borders around clusters
clusters.list = rect.hclust(cc, k = 4, border="blue")

# cut dendrogram 
clusters = cutree(cc, k = 4)

# plot graph with clusters
plot(g, vertex.color=clusters, layout=coords)

