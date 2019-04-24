library(igraph)


epidemic_iteration("~/Desktop/sir/epidemic_model.csv","~/Desktop/sir/iteration_0.csv");
epidemic_iteration("~/Desktop/sir/epidemic_model.csv","~/Desktop/sir/iteration_2.csv");
epidemic_iteration("~/Desktop/sir/epidemic_model.csv","~/Desktop/sir/iteration_1.csv");
epidemic_iteration("~/Desktop/sir/epidemic_model.csv","~/Desktop/sir/iteration_3.csv");
epidemic_iteration("~/Desktop/sir/epidemic_model.csv","~/Desktop/sir/iteration_4.csv");
epidemic_iteration("~/Desktop/sir/epidemic_model.csv","~/Desktop/sir/iteration_5.csv");
epidemic_iteration("~/Desktop/sir/epidemic_model.csv","~/Desktop/sir/iteration_6.csv");
epidemic_iteration("~/Desktop/sir/epidemic_model.csv","~/Desktop/sir/iteration_7.csv");
epidemic_iteration("~/Desktop/sir/epidemic_model.csv","~/Desktop/sir/iteration_8.csv");
epidemic_iteration("~/Desktop/sir/epidemic_model.csv","~/Desktop/sir/iteration_9.csv");
epidemic_iteration("~/Desktop/sir/epidemic_model.csv","~/Desktop/sir/iteration_10.csv");

epidemic_iteration <- function(networkFile, modelFile){

    df<-read.csv(networkFile, header = F, sep = ";", dec = ".")
    g <- graph_from_data_frame(d = df, directed = F)

    it0 <- as.data.frame( read.csv2(modelFile,header=T,sep=";"));
    colors = get_colors(it0, V(g));
    #help(layout_in_circle)
    
    #l <- layout_in_circle(g);
    minC <- rep(-Inf, vcount(g))
    maxC <- rep(Inf, vcount(g))
    minC[1] <- maxC[1] <- 0
    l <- layout_with_fr(g,minx=minC, maxx=maxC,miny=minC, maxy=maxC);
    plot(g, vertex.color = colors, layout=l,rescale=F,xlim=range(l[,1]), ylim=range(l[,2]), vertex.size=degree(g)*5);
    
    
    #plot(g, layout=co, vertex.size=30, edge.arrow.size=0.2,
    # vertex.label=c("ego", rep("", vcount(g)-1)), rescale=FALSE,
    # xlim=range(co[,1]), ylim=range(co[,2]), vertex.label.dist=0, vertex.label.color="red")
}   

get_color <- function(v, itInfo) {
 color <- ifelse(v %in% itInfo$Suspected, "yellow", ifelse(v %in% itInfo$Infected, "red", "green"));
 return(color);
}

get_colors <- function(itInfoP, vertices){
colors <- sapply(vertices,get_color, itInfo=itInfoP);
table(colors);
return(colors);
}
