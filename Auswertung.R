#par(mfrow=c(1,length(folders)))
#for(i in folders){
#  filename = paste(i,"/Signal.csv", sep = "")
#  myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
#  datavectors4 <- as.numeric(gsub("dB","", unlist(myData[,6], use.names = FALSE)))
#  boxplot(datavectors4, add=TRUE, main="Full RSRP",ylim=c(-6,12))
#}

folders <- list.dirs(path = "~/Messungen/", full.names = TRUE, recursive = FALSE )
par(mfrow=c(1,1))
filename = paste(folders[1],"/Signal.csv", sep = "")
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv1 <- as.numeric(gsub("dB","", unlist(myData[,6], use.names = FALSE)))

filename = paste(folders[2],"/Signal.csv", sep = "")
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv2 <- as.numeric(gsub("dB","", unlist(myData[,6], use.names = FALSE)))

filename = paste(folders[3],"/Signal.csv", sep = "")
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv3 <- as.numeric(gsub("dB","", unlist(myData[,6], use.names = FALSE)))

filename = paste(folders[4],"/Signal.csv", sep = "")
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv4 <- as.numeric(gsub("dB","", unlist(myData[,6], use.names = FALSE)))

filename = paste(folders[5],"/Signal.csv", sep = "")
myData <- read.csv(file=filename,header=FALSE,sep=";",stringsAsFactors=FALSE)
dv5 <- as.numeric(gsub("dB","", unlist(myData[,6], use.names = FALSE)))

full <- c(dv1,dv2,dv3,dv4,dv5)

dvfull <- cbind(dv1,dv2,dv3,dv4,dv5,full)
names <- c("Messung 1","Messung 2","Messung 3","Messung 4","Messung 5","Gesamt")
colnames(dvfull) <- names
boxplot(dvfull,  main="Full RSRP")