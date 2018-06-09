Netem = read.table("Documents/Prototyp/Tool1/Validationtool/outputtraces/Val_Netem_GilbertElliot_20_70_95_3.txt") #change file here to plot the other datasets
Tool = read.table("Documents/Prototyp/Tool1/Validationtool/outputtraces/Val_Tool_GilbertElliot_20_70_95_3.txt")  #change file here to plot the other datasets
yGEN = Netem[,1]
yGET = Tool[,1]
FnGEN <- ecdf(yGEN)
FnGET <- ecdf(yGET)
x <- c(seq(-10,60,by=10))
plot(FnGEN, lty=2, lwd=3, verticals=TRUE, do.points=FALSE,  xlim=c(-10,60), col="red", main="ECDF of Gilbert-Elliot", xlab="Burstsize [Number of Packets] (Negative = Loss, Positive = No Loss)", sub="Used Model-Parameters: p=20%, r=70% 1-h=95%, 1-k=3%", ylab="Cumulative Probability")
plot(FnGET, lty=3, lwd=3, verticals=TRUE, do.points=FALSE, add=TRUE, col="blue")
legend(48, 0.1, lwd=2, lty=c(2,3), col=c("red","blue"), legend=c("Netem","Our Tool"), bty="n")
abline(v=0, lty=3)
ks.test(yGEN, yGET, alternative="two.sided") #Kolmogorow-Smirnow Test