tconfintlow <- function( gamma, x) {return(mean(x)+qt((1 - gamma ) / 2 ,length (x) - 1)* sd(x)/sqrt(length (x) ) )  }
tconfintup  <- function( gamma, x) {return(mean(x)+qt(1-(1-gamma)/2 ,length(x)-1)*sd(x)/sqrt(length  (x) ))}

#NTP Slow Sync
filename <- "~/Schreibtisch/NTPLangsam/result.csv"
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv1temp <- myData[,1]
dv1low <-tconfintlow (0.95 , dv1temp) 
dv1up <-tconfintup (0.95 , dv1temp) 

#PTPD
filename <- "~/Schreibtisch/PTP_Zeitsynchro/result.csv"
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv2temp <- myData[,1]
dv2low <-tconfintlow (0.95 , dv2temp) 
dv2up <-tconfintup (0.95 , dv2temp) 

#NTP IBURST
filename <- "~/Schreibtisch/NTPFast/result.csv"
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv3temp <- myData[,1]
dv3low <-tconfintlow (0.95 , dv3temp) 
dv3up <-tconfintup (0.95 , dv3temp) 

names <- c("NTP Slow Sync", "NTP iBurst", "PTPd")

dvfull <- cbind(dv1temp,dv3temp,dv2temp)
colnames(dvfull) <- names

boxplot(dvfull, ylab="Abweichung vom Ping (ms)",  main="Synchronitätstest")
lines(c(0.6,1.4),c(dv1up,dv1up), col="red")
lines(c(0.6,1.4),c(dv1low,dv1low), col="red")
lines(c(2.6,3.4),c(dv2up,dv2up), col="red")
lines(c(2.6,3.4),c(dv2low,dv2low), col="red")
lines(c(1.6,2.4),c(dv3up,dv3up), col="red")
lines(c(1.6,2.4),c(dv3low,dv3low), col="red")
legend("top", lwd=1,lty=1,col="red",legend="95% Konfidenzintervall")
