# Quelle: https://www.uni-ulm.de/fileadmin/website_uni_ulm/mawi.inst.110/lehre/ss09/stoch1/R-skript.pdf
tconfintlow <- function( gamma, x) {return(mean(x)+qt((1 - gamma ) / 2 ,length (x) - 1)* sd(x)/sqrt(length (x) ) )  }
tconfintup  <- function( gamma, x) {return(mean(x)+qt(1-(1-gamma)/2 ,length(x)-1)*sd(x)/sqrt(length  (x) ))}

folders <- list.dirs(path = "~/Messungen/Messung 3", full.names = TRUE, recursive = FALSE )
par(mfrow=c(1,1))
filename = paste("~/Messungen/Messung 3/Messung1/Ergebnis/download.csv", sep = "")
myData <- read.table(file=filename)
dv1 <- myData[,1] / 1024 /1024
dv1low <-tconfintlow (0.95 , dv1) 
dv1up <-tconfintup (0.95 , dv1) 
W
dvfull <- cbind(dv1)
names <- c("Messung")
colnames(dvfull) <- names


boxplot(dv1, ylab="Bandbreite (MBit/s)", main="Bandwidth Download - Osnabrück - PG Raum ")
lines(c(0.5,1.5),c(dv1up,dv1up), col="red")
lines(c(0.5,1.5),c(dv1low,dv1low), col="red")
legend("bottomright", lwd=1,lty=1,col="red",legend="Konfidenzintervall")
