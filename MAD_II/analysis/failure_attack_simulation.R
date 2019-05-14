library(igraph)
library(testit)

randInt <- function(lowerBound, upperBound) {
    return(sample(c(lowerBound:upperBound), 1))
}

failureStep <- function(graph) {
    vertexCount <- length(V(graph))
    edgeCount <- length(E(graph, directed = F))
    # print(sprintf("Before failure step:    Vertex count: %i    Edge count: %i", vertexCount, edgeCount))

    randomVertexIndex <- randInt(0, vertexCount - 1)
    vertexToDelete = V(graph)[randomVertexIndex + 1]

    # print(sprintf("Going to delete vertex: %i", vertexToDelete))

    graph <- delete.vertices(graph, vertexToDelete)

    afterVertexCount <- length(V(graph))
    edgeCount <- length(E(graph, directed = F))
    # print(sprintf("After failure step:    Vertex count: %i    Edge count: %i", afterVertexCount, edgeCount))

    assert("Vertex wasn't deleted", afterVertexCount < vertexCount & afterVertexCount + 1 == vertexCount)
    return(graph)
}

attackStep <- function(graph) {
    vertexToDelete <- V(graph)$name[degree(graph) == max(degree(graph))]
    if (length(vertexToDelete) <= 0) {
        return(graph)
    }
    if (length(vertexToDelete) > 1) {
        vertexToDelete = vertexToDelete[1]
    }

    vertexCount <- length(V(graph))

    graph <- delete.vertices(graph, vertexToDelete)

    afterVertexCount <- length(V(graph))
    assert("Vertex wasn't deleted", afterVertexCount < vertexCount & afterVertexCount + 1 == vertexCount)
    # print(sprintf("went from %i to %i", vertexCount, afterVertexCount))
    return(graph)
}

createLineChart <- function(attackDf, failureDf, ylabText, legendPos = "bottomleft") {
    yMinDeg = floor(min(min(attackDf), min(failureDf)));
    yMaxDeg = ceiling(max(max(attackDf), max(failureDf)));
    xMax = max(length(attackDf), length(failureDf));
    plot(0, 0, xlab = "Odebraných vrcholù", ylab = ylabText,
    xlim = c(0, xMax),
    ylim = c(yMinDeg, yMaxDeg));
    lines(attackDf, col = "red");
    lines(failureDf, col = "blue");
    legend(legendPos, legend = c("Útok", "Selhání"), col = c("red", "blue"), lty = 1, cex = 0.8);
}

simulationFailureAndAttack <- function(graph, iterationCount) {

    # 10 rows:
    # d = data.frame(x = rep(0, 10), y = rep(0, 10), z = rep(0, 10))
    #And in the loop(index i):
    #d[i,] = c(x, y, z)

    simFailG <- graph
    simAttackG <- graph

    simFailData = data.frame(removed = rep(0, iterationCount),
                            avgDeg = rep(0, iterationCount),
                            avgDist = rep(0, iterationCount),
                            componentCount = rep(0, iterationCount),
                            componentSize = rep(0, iterationCount))

    simAttackData = data.frame(removed = rep(0, iterationCount),
                            avgDeg = rep(0, iterationCount),
                            avgDist = rep(0, iterationCount),
                            componentCount = rep(0, iterationCount),
                            componentSize = rep(0, iterationCount))


    for (it in c(1:iterationCount)) {

        simAttackG <- attackStep(simAttackG)
        simFailG <- failureStep(simFailG)

        aAvgDeg <- mean(degree(simAttackG))
        fAvgDeg <- mean(degree(simFailG))

        aAvgDist <- average.path.length(simAttackG, directed = F)
        fAvgDist <- average.path.length(simFailG, directed = F)

        aC <- components(simAttackG)
        fC <- components(simFailG)

        aCc <- aC$no
        fCc <- fC$no

        aMCS <- max(aC$csize)
        fMCS <- max(fC$csize)

        simAttackData[it,] <- c(it, aAvgDeg, aAvgDist, aCc, aMCS)
        simFailData[it,] <- c(it, fAvgDeg, fAvgDist, fCc, fMCS)

        message(sprintf("Finished iteration %i", it))
    }
    print(simAttackData)
    print(simFailData)


    pom = layout(mat = matrix(1:4, 2, 2, byrow = FALSE));
    # layout.show(pom);    

    createLineChart(simAttackData$componentCount, simFailData$componentCount, "Poèet komponent");
    createLineChart(simAttackData$componentSize, simFailData$componentSize, "Velikost komponenty");
    createLineChart(simAttackData$avgDist, simFailData$avgDist, "Prùmìrná vzdálenost");
    createLineChart(simAttackData$avgDeg, simFailData$avgDeg, "Prùmìrný stupeò");
    title("Porovnání Útoku a Selhání v síti", outer = T, line = -2);
}