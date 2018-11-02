fast = read.table("Messungen/Messung3/Messung1/Ergebnis/fast/downloadLossVal.txt") #change file here to plot the other datasets
slow = read.table("Messungen/Messung3/Messung1/Ergebnis/slow/downloadLossVal.txt")  #change file here to plot the other datasets
yGEN = fast[,1]
yGET = slow[,1]
FnGEN <- ecdf(yGEN)
FnGET <- ecdf(yGET)
x <- c(seq(-10,60,by=10))
plot(FnGEN, lty=2, lwd=3, verticals=TRUE, do.points=FALSE, col="red", main="ECDF-Packetloss Testmessung PG-Raum", xlab="Burstsize [Number of Packets] (Negative = Loss, Positive = No Loss)", ylab="Cumulative Probability")
plot(FnGET, lty=3, lwd=3, verticals=TRUE, do.points=FALSE, add=TRUE, col="blue")
legend("bottomright", lwd=2, lty=c(2,3), col=c("red","blue"), legend=c("Fast CBR Downloadloss","Slow CBR Download"))
abline(v=0, lty=3)
#ks.test(yGEN, yGET, alternative="two.sided") #Kolmogorow-Smirnow Test