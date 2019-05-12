library(igraph)
library(stringr)

getwd()
list.files("sir/")

epidemicAnimation("gr_qc_collab.csv", "sir/iteration_0.csv", 5)

epidemicAnimation <- function(networkFile, firstSirFile, iterationCount, networkSeparator="") {
    endIndex <- as.numeric(gregexpr(pattern = "_0.csv", firstSirFile))
    nameBeg <- substring(firstSirFile, 0, endIndex-1)

    df <- read.csv(networkFile, header = F, sep = networkSeparator, dec = ".")
    g <- graph_from_data_frame(d = df, directed = F)
    vert <- V(g)
    vc <- vcount(g);
    for (time in c(0:iterationCount)) {
        itFile <- sprintf("%s_%i.csv", nameBeg, time)
        message(sprintf("It file: %s", itFile))

        itInfo <- as.data.frame(read.csv2(itFile, header = T, sep = ";"));
        colors = get_colors(itInfo, vert);

        minC <- rep(-Inf, vc)
        maxC <- rep(Inf, vc)
        minC[1] <- maxC[1] <- 0

        l <- layout_with_fr(g, minx = minC, maxx = maxC, miny = minC, maxy = maxC);
        plot(g, vertex.color = colors, layout = l, rescale = F, xlim = range(l[, 1]), ylim = range(l[, 2]), vertex.size = degree(g) * 5);
        invisible(readline(prompt = "Press [enter] to continue"))
    }
}

#epidemic_iteration <- function(networkFile, modelFile) {

    #df <- read.csv(networkFile, header = F, sep = ";", dec = ".")
    #g <- graph_from_data_frame(d = df, directed = F)

    #it0 <- as.data.frame(read.csv2(modelFile, header = T, sep = ";"));
    #colors = get_colors(it0, V(g));

    #minC <- rep(-Inf, vcount(g))
    #maxC <- rep(Inf, vcount(g))
    #minC[1] <- maxC[1] <- 0
    #l <- layout_with_fr(g, minx = minC, maxx = maxC, miny = minC, maxy = maxC);
    #plot(g, vertex.color = colors, layout = l, rescale = F, xlim = range(l[, 1]), ylim = range(l[, 2]), vertex.size = degree(g) * 5);
    ##plot(g, layout=co, vertex.size=30, edge.arrow.size=0.2,
    ## vertex.label=c("ego", rep("", vcount(g)-1)), rescale=FALSE,
    ## xlim=range(co[,1]), ylim=range(co[,2]), vertex.label.dist=0, vertex.label.color="red")
#}

get_color <- function(v, itInfo) {
    color <- ifelse(v %in% itInfo$Suspected, "yellow", ifelse(v %in% itInfo$Infected, "red", "green"));
    return(color);
}

get_colors <- function(itInfoP, vertices) {
    colors <- sapply(vertices, get_color, itInfo = itInfoP);
    table(colors);
    return(colors);
}
