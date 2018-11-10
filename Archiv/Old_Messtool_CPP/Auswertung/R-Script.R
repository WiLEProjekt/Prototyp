bandbreite <- read.table("Documents/Prototyp/Messtool/Server/build/bandwidths.txt")
x <- seq(1,5000)
plot(x, bandbreite[,1], ylim=c(10000, 200000))
abline(h=36379, lty=1, lwd=2)
abline(h=51379, lty=3, lwd=2)
abline(h=21379, lty=3, lwd=2)
abline(h=35985, lty=1, col="red") #Median des Epsilonbereichs
abline(h=41204, lty=1, col="blue") # Gesamtmedian
median(bandbreite[,1])