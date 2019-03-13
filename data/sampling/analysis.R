library(igraph)
library(ggplot2)

#install.packages('igraph');
#install.packages('ggplot2');

setwd("D:/gitrepos/dataAnalysisCourse/data/sampling");
getwd();
#BA
ba      = "ba.csv";
baRNS   = "baRNS.csv";
baRES   = "baRES.csv";

df      = as.data.frame(read.csv(ba,    header = F, sep = ";"));
dfRNS   = as.data.frame(read.csv(baRNS, header = F, sep = ";"));
dfRES   = as.data.frame(read.csv(baRES, header = F, sep = ";"));

g       = graph_from_data_frame(df,     directed = F);
gRNS    = graph_from_data_frame(dfRNS,  directed = F);
gRES    = graph_from_data_frame(dfRES,  directed = F);

gDeg       = degree(g,     normalized = T);
gDegRNS    = degree(gRNS,  normalized = T);
gDegRES    = degree(gRES,  normalized = T);

cDD     = degree_distribution(g,        cumulative = T, mode="all");
cDDRNS  = degree_distribution(gRNS,     cumulative = T, mode="all");
cDDRES  = degree_distribution(gRES,     cumulative = T, mode="all");

x = data.frame(degree=gDeg, freq=cDD);

cDD_DF      = data.frame(d = c(0:(length(cDD) - 1)), cumFreq = 1-cDD);
cDDRNS_DF   = data.frame(d = c(0:(length(cDDRNS) - 1)), cumFreq = 1-cDDRNS);
cDDRES_DF   = data.frame(d = c(0:(length(cDDRES) - 1)), cumFreq = 1-cDDRES);


length(gDeg)
length(cDD)

ggplot(cDD_DF, aes(x = gDeg, y = cumFreq)) + geom_line() + 
    scale_x_continuous("Stupen") + scale_y_continuous("Frekvence") +
    ggtitle("Kumulativní distribuce stupnu") +
    theme(plot.title = element_text(hjust = 0.5, size = 30));
    
ggplot(cDDRNS_DF, aes(x = d, y = cumFreq)) + geom_line() + 
    scale_x_continuous("Stupen") + scale_y_continuous("Frekvence") +
    ggtitle("Kumulativní distribuce stupnu") +
    theme(plot.title = element_text(hjust = 0.5, size = 30));
    
ggplot(cDDRES_DF, aes(x = d, y = cumFreq)) + geom_line() + 
    scale_x_continuous("Stupen") + scale_y_continuous("Frekvence") +
    ggtitle("Kumulativní distribuce stupnu") +
    theme(plot.title = element_text(hjust = 0.5, size = 30));


plot(x = 0:max(degree(g)), y = 1 - cDD, type = "l", col = "red", xlab = "Degree", ylab = "Cumulative Frequency")
lines(x = 0:max(degree(gRNS)), y = 1 - cDDRNS, col = "green")
lines(x = 0:max(degree(gRES)), y = 1 - cDDRES, col = "blue")
legend("bottomright",
       c("Barabasi-Albert", "Sample RNS", "Sample RDS"),
       fill = c("red", "green", "blue"))

plot(x = 0:max(degree(g)), y = 1 - cDD, type = "l", col = "red", xlab = "Degree", ylab = "Cumulative Frequency")
lines(x = 0:max(degree(gRNS)), y = 1 - cDDRNS, col = "green")
lines(x = 0:max(degree(gRES)), y = 1 - cDDRES, col = "blue")
legend("bottomright",
       c("Barabasi-Albert", "Sample RNS", "Sample RDS"),
       fill = c("red", "green", "blue"))
