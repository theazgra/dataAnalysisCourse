setwd("/home/mor0146/github/dataAnalysisCourse/data/failureAndAttack/withDistance")
createReport(withDistance=T, "ba_attack.csv", "ba_fail.csv", "Scale Free network");
createReport(withDistance=T, "random_attack.csv", "random_fail.csv", "Random network");
createReport(withDistance=T, "usairport_attack.csv", "usairport_fail.csv", "US Airport network");

setwd("/home/mor0146/github/dataAnalysisCourse/data/failureAndAttack/withoutDistance")
createReport(withDistance=F, "ba_attack.csv", "ba_fail.csv", "Scale Free network");
createReport(withDistance=F, "random_attack.csv", "random_fail.csv", "Random network");
createReport(withDistance=F, "usairport_attack.csv", "usairport_fail.csv", "US Airport network");


createReport <- function(attackFile, failureFile, titleText, withDistance) {
    dfAttack <- read.csv2(attackFile, header = T, sep = ";", dec = ".");
    dfFailure <- read.csv2(failureFile, header = T, sep = ";", dec = ".");

    pom = layout(mat = matrix(1:4, 2, 2, byrow = FALSE));
    #layout.show(pom);    
    
    createLineChart(dfAttack$ComponentCount, dfFailure$ComponentCount, "Po?et komponent");
    createLineChart(dfAttack$MaxComponentSize, dfFailure$MaxComponentSize, "Velikost komponenty");

    if (withDistance){
        createLineChart(dfAttack$AverageDistance, dfFailure$AverageDistance, "Pr?m?rná vzdálenost");
    }
    createLineChart(dfAttack$AverageDegree, dfFailure$AverageDegree, "Pr?m?rný stupe?");
    title(titleText, outer=T,line=-2);
}

createLineChart <- function(attackDf, failureDf, ylabText, legendPos="bottomleft") {
    yMinDeg = floor(min(min(attackDf), min(failureDf)));
    yMaxDeg = ceiling(max(max(attackDf), max(failureDf)));
    xMax = max(length(attackDf), length(failureDf));
    plot(0, 0, xlab = "Odebraných vrchol?", ylab = ylabText,
    xlim = c(0, xMax),
    ylim = c(yMinDeg, yMaxDeg));
    lines(attackDf, col = "red");
    lines(failureDf, col = "blue");
    legend(legendPos, legend = c("Útok", "Selhání"), col = c("red", "blue"),lty = 1, cex = 0.8);
}
