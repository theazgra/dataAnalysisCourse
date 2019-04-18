library("igraph");

setwd("/home/mor0146/github/dataAnalysisCourse/data");
getwd();

list.files(getwd());

file <- read.csv("linkSelection.csv",header=F,sep=";");
g <- graph_from_data_frame(d=file,directed=F);

degDist <- degree_distribution(g,cumulative=T,mode="total");
degDist
help(degree_distribution)

plot(degDist,log="xy")
