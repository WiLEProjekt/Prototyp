Netem = read.table("Documents/Prototyp/Tool1/Validationtool/outputtraces/GE/Val_GE_Orig.txt") #change file here to plot the other datasets
one = read.table("Documents/Prototyp/Tool1/Validationtool/outputtraces/GE/Val_GE1.txt")  #change file here to plot the other datasets
four = read.table("Documents/Prototyp/Tool1/Validationtool/outputtraces/GE/Val_GE4.txt")  #change file here to plot the other datasets
seven = read.table("Documents/Prototyp/Tool1/Validationtool/outputtraces/GE/Val_GE7.txt")  #change file here to plot the other datasets
ten = read.table("Documents/Prototyp/Tool1/Validationtool/outputtraces/GE/Val_GE10.txt")  #change file here to plot the other datasets
thirteen = read.table("Documents/Prototyp/Tool1/Validationtool/outputtraces/GE/Val_GE13.txt")  #change file here to plot the other datasets
sixteen = read.table("Documents/Prototyp/Tool1/Validationtool/outputtraces/GE/Val_GE16.txt")  #change file here to plot the other datasets
nineteen = read.table("Documents/Prototyp/Tool1/Validationtool/outputtraces/GE/Val_GE19.txt")  #change file here to plot the other datasets
yGEN = Netem[,1]
yGET1 = one[,1]
yGET4 = four[,1]
yGET7 = seven[,1]
yGET10 = ten[,1]
yGET13 = thirteen[,1]
yGET16 = sixteen[,1]
yGET19 = nineteen[,1]
FnGEN <- ecdf(yGEN)
FnGET1 <- ecdf(yGET1)
FnGET4 <- ecdf(yGET4)
FnGET7 <- ecdf(yGET7)
FnGET10 <- ecdf(yGET10)
FnGET13 <- ecdf(yGET13)
FnGET16 <- ecdf(yGET16)
FnGET19 <- ecdf(yGET19)
x <- c(seq(-10,60,by=10))
plot(FnGEN, lty=1, lwd=4, verticals=TRUE, do.points=FALSE,  xlim=c(-10,60), col="black", main="ECDF - parameter estimation Gilbert-Elliot", xlab="Burstsize [Number of Packets] (Negative = Loss, Positive = No Loss)", sub="Used Model-Parameters: p=4%, r=90% 1-h=98%, 1-k=5%", ylab="Cumulative Probability")
#plot(FnGET1, lty=1, lwd=3, verticals=TRUE, do.points=FALSE, add=TRUE, col="orange")
plot(FnGET4, lty=1, lwd=4, verticals=TRUE, do.points=FALSE, add=TRUE, col="blue")
#plot(FnGET7, lty=3, lwd=3, verticals=TRUE, do.points=FALSE, add=TRUE, col="back")
plot(FnGET10, lty=1, lwd=4, verticals=TRUE, do.points=FALSE, add=TRUE, col="chartreuse4")
#plot(FnGET13, lty=3, lwd=3, verticals=TRUE, do.points=FALSE, add=TRUE, col="brown")
plot(FnGET16, lty=1, lwd=4, verticals=TRUE, do.points=FALSE, add=TRUE, col="red")
#plot(FnGET19, lty=3, lwd=3, verticals=TRUE, do.points=FALSE, add=TRUE, col="orange")
legend(53, 0.13, lwd=4, lty=1, col=c("black","blue", "chartreuse4", "red" ), legend=c("Original Trace","gMin = 4", "gMin = 10", "gMin = 16"), bty="n", cex=1.1)
abline(v=0, lty=3)
#ks.test(yGEN, yGET1, alternative="two.sided") #Kolmogorow-Smirnow Test