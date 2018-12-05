
mainDf <- as.data.frame(read.csv("../../../data/project/googleplaystore.csv", header = T, sep = ","))

# Column names.
names(mainDf)

# Dimensions.
dim(mainDf)

# Get attributes of specific column.
attributes(mainDf$Category) # vraci atributy

summary(mainDf)

categoryTable <- table(mainDf$Category)
categoryTable
barplot(categoryTable)

hist(mainDf$Rating)


hist(df_num$Humidity)
plot(density(df_num$Humidity))
hist(df_num$Humidity, prob=TRUE, col="grey")
lines(density(df_num$Humidity), col="blue", lwd=2) 

hist(df_num$Temperature) #histogram teploty
plot(density(df_num$Temperature))  # hustota 
hist(df_num$Temperature, prob=TRUE, col="grey")
lines(density(df_num$Temperature), col="blue", lwd=2) 

mean(df_num$Humidity) # prumer
median(df_num$Humidity) # median
sd(df_num$Humidity)  # smerodatna odchylka
var(df_num$Humidity) # royptyl