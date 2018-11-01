#df <- as.data.frame(read.csv("./weather_nominal.csv", header = T, sep = ";")) # nacteni csv souboru, \


dfSepLen <- as.data.frame(read.csv("../../../results/distribution_0.csv", sep = ";", dec = ","))
plot(dfSepLen)

iris_dataset <- as.data.frame(read.csv("../../../data/iris.csv", sep = ",", dec = "."))
iris_dataset <- iris_dataset[, -5]
iris_dataset

kmeans(iris_dataset, centers = 3)

