# Quelle: https://www.uni-ulm.de/fileadmin/website_uni_ulm/mawi.inst.110/lehre/ss09/stoch1/R-skript.pdf
tconfintlow <- function( gamma, x) {return(mean(x)+qt((1 - gamma ) / 2 ,length (x) - 1)* sd(x)/sqrt(length (x) ) )  }
tconfintup  <- function( gamma, x) {return(mean(x)+qt(1-(1-gamma)/2 ,length(x)-1)*sd(x)/sqrt(length  (x) ))}

folders <- list.dirs(path = "~/Messungen/", full.names = TRUE, recursive = FALSE )
par(mfrow=c(1,1))
filename = paste(folders[1],"/Signal.csv", sep = "")
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv1 <- as.numeric(gsub("dBm","", unlist(myData[,4], use.names = FALSE)))
dv1low <-tconfintlow (0.95 , dv1) 
dv1up <-tconfintup (0.95 , dv1) 

filename = paste(folders[2],"/Signal.csv", sep = "")
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv2 <- as.numeric(gsub("dBm","", unlist(myData[,4], use.names = FALSE)))
dv2low <-tconfintlow (0.95 , dv2) 
dv2up <-tconfintup (0.95 , dv2) 

filename = paste(folders[3],"/Signal.csv", sep = "")
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv3 <- as.numeric(gsub("dBm","", unlist(myData[,4], use.names = FALSE)))
dv3low <-tconfintlow (0.95 , dv3) 
dv3up <-tconfintup (0.95 , dv3) 

filename = paste(folders[4],"/Signal.csv", sep = "")
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv4 <- as.numeric(gsub("dBm","", unlist(myData[,4], use.names = FALSE)))
dv4low <-tconfintlow (0.95 , dv4) 
dv4up <-tconfintup (0.95 , dv4) 

filename = paste(folders[5],"/Signal.csv", sep = "")
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv5 <- as.numeric(gsub("dBm","", unlist(myData[,4], use.names = FALSE)))
dv5low <-tconfintlow (0.95 , dv5) 
dv5up <-tconfintup (0.95 , dv5) 

full <- c(dv1,dv2,dv3,dv4,dv5)
dvfulllow <-tconfintlow (0.95 , dv4) 
dvfullup <-tconfintup (0.95 , dv4) 

length(dv1) = length(full)
length(dv2) = length(full)
length(dv3) = length(full)
length(dv4) = length(full)
length(dv5) = length(full)


dvfull <- cbind(dv1,dv2,dv3,dv4,dv5,full)
names <- c("Messung 1","Messung 2","Messung 3","Messung 4","Messung 5","Gesamt")
colnames(dvfull) <- names


boxplot(dvfull, ylab="RSRP (dBm)", main="RSRP - Osnabrück - Siedlung Westerberg - Urban- 52.283902/8.032466")
lines(c(0.5,1.5),c(dv1up,dv1up), col="red")
lines(c(0.5,1.5),c(dv1low,dv1low), col="red")
lines(c(1.5,2.5),c(dv2up,dv2up), col="red")
lines(c(1.5,2.5),c(dv2low,dv2low), col="red")
lines(c(2.5,3.5),c(dv3up,dv3up), col="red")
lines(c(2.5,3.5),c(dv3low,dv3low), col="red")
lines(c(3.5,4.5),c(dv4up,dv4up), col="red")
lines(c(3.5,4.5),c(dv4low,dv4low), col="red")
lines(c(4.5,5.5),c(dv5up,dv5up), col="red")
lines(c(4.5,5.5),c(dv5low,dv5low), col="red")
lines(c(5.5,6.5),c(dvfullup,dvfullup), col="red")
lines(c(5.5,6.5),c(dvfulllow,dvfulllow), col="red")
legend("top", lwd=1,lty=1,col="red",legend="Konfidenzintervall")
#abline(h=dv1low,col="red")
