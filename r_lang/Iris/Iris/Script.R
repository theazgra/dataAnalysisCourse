#df <- as.data.frame(read.csv("./weather_nominal.csv", header = T, sep = ";")) # nacteni csv souboru, \


dfSepLen <- as.data.frame(read.csv("../../../results/distribution_0.csv", sep = ";", dec = ","))
plot(dfSepLen)