# dunno
#setwd("/home/mor0146/github/dataAnalysisCourse/data/project")
setwd("D:/gitrepos/dataAnalysisCourse/data/project")

sirData <- read.csv("sir/sir_result.csv", header = T, sep = ";")

vertexCount <- sum(sirData[1,])
sirRelData <- data.frame(it = sirData$Iteration, s = sirData$Suspected / vertexCount,
                         i = sirData$Infected / vertexCount, r = sirData$Recovered / vertexCount)


sirSum <- data.frame(it = sirData$Iteration, sum <- (sirRelData$s + sirRelData$i + sirRelData$r))

View(sirSum)

cols <- c("blue", "red", "green")

plot(-1, -1,xlab = "Iterace", ylab = "Procento vrcholù", xlim = c(0, 30), ylim = c(0, 1));
lines(x = sirRelData$it, y = sirRelData$s, col = cols[1], lwd = 2)
lines(x = sirRelData$it, y = sirRelData$i, col = cols[2], lwd = 2)
lines(x = sirRelData$it, y = sirRelData$r, col = cols[3], lwd = 2)
legend("topright", inset = c(0,0.2), legend = c("Suspected", "Infected", "Recovered"), col = cols, lty = 1, cex = 1,lwd = 2);
