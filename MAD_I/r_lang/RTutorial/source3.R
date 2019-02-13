df<-read.csv("./data/KarateClub.csv", header = F, sep = ";", dec = ",")
df

if (!require("igraph")) 
  install.packages("igraph")
library(igraph) # verze 1.0.1

g1 <- graph_from_data_frame(d = df, directed = F)


diameter(g1, directed = F) # prumer == nejdelsi nejkratsi cesta, hodnota
diam <- get_diameter(g1, directed = F) # vrcholy tvorici prvni nalezenou cestu dane delky
as.vector(diam) # z objektu tridy igraph udelame obyc. vektor
# vykreslime
vcol <- rep("lightblue", vcount(g1))
vcol[diam] <- "gold"
ecol <- rep("black", ecount(g1)) # pouzij cernou na vsechny hrany grafu
ecol[E(g1, path = diam)] <- "red"
# E(g1, path = diam) nalezne hrany tvorici cestu, nyni reprezent. jako 'diam'
plot(g1, vertex.color = vcol, edge.color = ecol, edge.arrow.mode = 0)

# prumerna vzdalenost == prumerna delka nejkratsi cesty
mean_distance(g1, directed = F) 
# matice vzdalenosti mezi vsemi dvojcemi vrcholu grafu
A <- distances(g1) 

# cesta mezi vybranou dvojici vrcholu
path1 <- shortest_paths(g1, from = V(g1)["1"], to = V(g1)["27"], output = "both") # "both path" vrcholy i hrany
ecol <- rep("black", ecount(g1))
ecol[unlist(path1$epath)] <- "red"
# Generujeme sirku hran tvoricich cestu
ew <- rep(1, ecount(g1))
ew[unlist(path1$epath)] <- 2
# Generujeme barvu vrcholu tvoricich cestu 
vcol <- rep("lightblue", vcount(g1))
vcol[unlist(path1$vpath)] <- "gold"
plot(g1, vertex.color = vcol, edge.color = ecol, edge.width = ew, edge.arrow.mode = 0)

# pocitano jako 1/suma(delek vsech cest z vrcholu i), pozor vysledek neni serazeny dle id vrcholu
centr <- closeness(g1, mode = "all", weights = NA) 
# pocitano jako (n-1)/suma(delek vsech cest z vrcholu i), pozor vysledek neni serazeny dle id vrcholu
centr_clo(g1, mode = "all", normalized = F)

# pocitano vlasni funkci jako n/suma(delek vsech cest z vrcholu i)
fce_clos <- function(x)
{
  cc <- numeric(34)
  souc <- 0
  for(i in 1: nrow(x))
  {
    
    cc[i] = 34 / (sum(A[i,]))
  }
  return(cc)
}

fce_clos(A)

plot(x = as.numeric(names(centr)), y = as.numeric(centr), xlab = "Degree", ylab = "Closeness")
names(which(centr == min(centr))) # najdi vrchol s nejmensi centralitou
names(which(centr == max(centr))) # nejvetsi


# vlastni funkce pro urceni cetnosti jednotlivych delek cest (z dolni trojuh. matice)
fce <- function(x)
{
  v <- numeric(max(x) + 1)
  for(i in 1: nrow(x))
  {
    for(j in 1 : i)  
    {
      v[A[i,j]+1] = v[A[i,j]+1]+1
    }
  }
  return(v)
}
# mohli bychom ji jeste upravit, aby se nepocitaly prvky na diagonale, tedy cesty delky 0 ....

# vykreslime cetnost, ziskame ji pomoci nasi fce
barplot(as.numeric(fce(A)), names.arg = c(0:max(A))) 


