library(igraph)
library(ggplot2)

#install.packages('igraph');
#install.packages('ggplot2');

#BA

ba      = "/home/mor0146/github/dataAnalysisCourse/data/sampling/random.csv";
baRNS   = "/home/mor0146/github/dataAnalysisCourse/data/sampling/randomRNS.csv";
baRES   = "/home/mor0146/github/dataAnalysisCourse/data/sampling/randomRES.csv";

df      = as.data.frame(read.csv(ba,    header = F, sep = ";"));
dfRNS   = as.data.frame(read.csv(baRNS, header = F, sep = ";"));
dfRES   = as.data.frame(read.csv(baRES, header = F, sep = ";"));

g       = graph_from_data_frame(df,     directed = F);
gRNS    = graph_from_data_frame(dfRNS,  directed = F);
gRES    = graph_from_data_frame(dfRES,  directed = F);

cDD     = degree_distribution(g,        cumulative = T, mode="all");
cDDRNS  = degree_distribution(gRNS,     cumulative = T, mode="all");
cDDRES  = degree_distribution(gRES,     cumulative = T, mode="all");

cDD_DF      = data.frame(d = c(0:(length(cDD) - 1)), cumFreq = 1-cDD);
cDDRNS_DF   = data.frame(d = c(0:(length(cDDRNS) - 1)), cumFreq = 1-cDDRNS);
cDDRES_DF   = data.frame(d = c(0:(length(cDDRES) - 1)), cumFreq = 1-cDDRES);



ggplot(cDD_DF, aes(x = d, y = cumFreq)) + geom_line() + 
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
