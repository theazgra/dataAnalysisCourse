setwd("D:/gitrepos/dataAnalysisCourse/data/failureAndAttack")

createReport("ba_attack.csv", "ba_fail.csv");
createReport("random_attack.csv", "random_fail.csv");
createReport("usairport_attack.csv", "usairport_fail.csv");


createReport <- function(attackFile, failureFile) {
    dfAttack <- read.csv2(attackFile, header = T, sep = ";", dec = ".");
    dfFailure <- read.csv2(failureFile, header = T, sep = ";", dec = ".");

    pom = layout(mat = matrix(1:4, 2, 2, byrow = FALSE));
    layout.show(pom);

    createLineChart(dfAttack$ComponentCount, dfFailure$ComponentCount, "Poèet komponent");
    createLineChart(dfAttack$MaxComponentSize, dfFailure$MaxComponentSize, "Velikost komponenty");
    createLineChart(dfAttack$AverageDistance, dfFailure$AverageDistance, "Prùmìrná vzdálenost");
    createLineChart(dfAttack$AverageDegree, dfFailure$AverageDegree, "Prùmìrný stupeò");
}

createLineChart <- function(attackDf, failureDf, ylabText, legendPos="bottomleft") {
    yMinDeg = floor(min(min(attackDf), min(failureDf)));
    yMaxDeg = ceiling(max(max(attackDf), max(failureDf)));
    xMax = max(length(attackDf), length(failureDf));
    plot(0, 0, xlab = "Iterace", ylab = ylabText,
    xlim = c(0, xMax),
    ylim = c(yMinDeg, yMaxDeg));
    lines(attackDf, col = "red");
    lines(failureDf, col = "blue");
    legend("bottomleft", legend = c("Útok", "Selhání"), col = c("red", "blue"),lty = 1, cex = 0.8);
}