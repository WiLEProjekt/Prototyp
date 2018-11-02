tconfintlow <- function( gamma, x) {return(mean(x)+qt((1 - gamma ) / 2 ,length (x) - 1)* sd(x)/sqrt(length (x) ) )  }
tconfintup  <- function( gamma, x) {return(mean(x)+qt(1-(1-gamma)/2 ,length(x)-1)*sd(x)/sqrt(length  (x) ))}


filename <- "~/Messungen/NTP_Sync_3Globale_Server.csv"
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv1temp <- myData[1,]
dv1 <- as.numeric(t(dv1temp))
dv1low <-tconfintlow (0.95 , dv1) 
dv1up <-tconfintup (0.95 , dv1) 
dv1low

filename <- "~/Messungen/result.csv"
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv2temp <- myData[1,]
dv2 <- as.numeric(t(dv2temp))
dv2low <-tconfintlow (0.95 , dv2) 
dv2up <-tconfintup (0.95 , dv2) 
dv2low

names <- c("NTP 3S","PTPd")


dvfull <- cbind(dv1,dv2)
colnames(dvfull) <- names

boxplot(dvfull, ylab="Abweichung vom Ping (ms)",  main="Synchronitätstest")
lines(c(0.75,1.25),c(dv1up,dv1up), col="red")
lines(c(0.75,1.25),c(dv1low,dv1low), col="red")
lines(c(1.75,2.25),c(dv2up,dv2up), col="red")
lines(c(1.75,2.25),c(dv2low,dv2low), col="red")
legend("top", lwd=1,lty=1,col="red",legend="95% Konfidenzintervall")
