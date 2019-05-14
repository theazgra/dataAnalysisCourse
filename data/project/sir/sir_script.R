# dunno
getwd()
setwd("/home/mor0146/github/dataAnalysisCourse/data/project")
sirData <- read.csv("sir/result_3.csv",header=T,sep=";")
View(sirData)

vc<-5242

plot(0,0,xlim = c(0,1005),ylim = c(0,1),type = "n")
lines(x=sirData$Iteration,y=sirData$Suspected/vc,col="yellow")
lines(x=sirData$Iteration,y=sirData$Infected/vc,col="red")
lines(x=sirData$Iteration,y=sirData$Recovered/vc,col="green")
