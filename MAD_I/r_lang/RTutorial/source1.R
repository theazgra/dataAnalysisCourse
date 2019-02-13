# ------- 1. priklad ---------------------------

a <- 1:10   # cela cisla od 1 do 10 (vektor)
b <- letters[1:5]  # vektor znaku (objekt letters je built-in vektor 26 znaku ang. abecedy)
save(a, b, file="./data/mydatafile.Rdata")
rm(a, b)  # zrusit a, b
load("./data/mydatafile.Rdata")
print(a)  # nebo jen zadat a
print(b)
class(b) # typ datoveho objektu

# ------- 2. priklad ---------------------------

var1 <- sample(5)  # vygeneruje nahodna cisla od 1 do 5
var2 <- var1 / 10 # artimeticke operace jako obvykle
var3 <- c("R", "and", "Data Mining", "Examples", "Case Studies")
df1 <- data.frame(var1, var2, var3)  # vytvoreni datove tabulky
names(df1) <- c("Var.Int", "Var.Num", "Var.Char")  # nastavime nazvy sloupcu (atributu)

write.csv(df1, "./data/mydatafile3.csv", row.names = FALSE)
df2 <- read.csv("./data/mydatafile3.csv")
print(df2) # nebo jen zadat df2
df2$Var.Int # vypis hodnot ve sloupci (atributu) Var.Int

# ------- 3. priklad ---------------------------

df<-as.data.frame(read.csv("../data/weather_nominal.csv", header = T, sep = ",")) # nacteni csv souboru, 
# casto je potreba zadat, zda chceme nacist i nazvy sloupcu a urcit oddelovac
names(df) # nazvy promennych (sloupcu)

df$Outlook 
df$Temperature
df$Humidity
df$Windy
df$Play

dim(df) # urceni rozmeru datove tabulky
str(df) # vraci strukturu tabulky
attributes(df) # vraci atributy
head(df, 5) # vypise prvnich 5 radku tabulky
tail(df, 2) # vypise posledni 2 radky tabulky
summary(df) # vraci frekvenci vyskytu kazde hodnoty daneho atributu (u numerickych jeste median, mean,....)
nrow(df) # pocet radku, pocet sloupcu ncol() 

t<-table(df$Outlook) # cetnost
t
barplot(t)
pie(t)

#######################

# nechceme atributy jako factor (jako kategorialni data), ale jako napr. retezec
df<-as.data.frame(read.csv("./weather_nominal.csv", header = T, sep = ";", stringsAsFactors = FALSE))
str(df)

# overeni klasifikace
p1 <-  subset(df, Outlook == "sunny" & Humidity == "high") # zikame podmnozinu vsech instanci
df[df$Outlook == "sunny" & df$Humidity == "high", 5] # zajima nas hodnota 5. atributu

p2 <-  subset(df, Outlook == "rainy" & Windy == "TRUE")
p3 <-  subset(df, Outlook == "overcast")
p4 <-  subset(df, Humidity == "normal")
df[df$Humidity == "normal",5]

table(df[,c("Outlook", "Play")])
table(df[,c("Temperature", "Play")])

# overeni asocoacnich pravidel
df[df$Temperature == "cool",3]
df[df$Humidity == "normal" & df$Windy == "FALSE", 5]


# ------- 4. priklad ---------------------------

df_num<-as.data.frame(read.csv("./weather.csv", header = T, sep = ";", stringsAsFactors = FALSE)) # nacteni csv souboru, 
str(df_num)

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
