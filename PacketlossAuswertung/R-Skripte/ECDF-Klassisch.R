Orig = read.table("Dokumente/FinaleLossAuswertung/28012019/OS_to_MS1/Auswertung_Rohdaten/OriginalECDF.txt") #change file here to plot the other datasets
GEKol = read.table("Dokumente/FinaleLossAuswertung/28012019/OS_to_MS1/Auswertung_Rohdaten/GilbertElliotECDFKolmogorov.txt")  #change file here to plot the other datasets
GESquared = read.table("Dokumente/FinaleLossAuswertung/28012019/OS_to_MS1/Auswertung_Rohdaten/GilbertElliotECDFLeastSquared.txt")
GKol = read.table("Dokumente/FinaleLossAuswertung/28012019/OS_to_MS1/Auswertung_Rohdaten/GilbertECDFKolmogorov.txt")
GSquared = read.table("Dokumente/FinaleLossAuswertung/28012019/OS_to_MS1/Auswertung_Rohdaten/GilbertECDFLeastSquared.txt")
OrigECDF <- ecdf(Orig[,1])
GEKolECDF <- ecdf(GEKol[,1])
GESquaredECDF <- ecdf(GESquared[,1])
GKolECDF <- ecdf(GKol[,1])
GSquaredECDF <- ecdf(GSquared[,1])
x <- c(seq(-10,60,by=10))
#par(mar=c(4.5,4.5,3,10))
plot(OrigECDF, lty=1, lwd=1, verticals=TRUE, do.points=FALSE, col="black", main="Comparison of ECDF-Distance-metrics", xlab="Burstsize [Number of Packets] (Negative = Loss, Positive = No Loss)", ylab="Cumulative Probability")
plot(GEKolECDF, lty=1, lwd=1, verticals=TRUE, do.points=FALSE, add=TRUE, col="blue")
plot(GESquaredECDF, lty=1, lwd=1, verticals=TRUE, do.points=FALSE, add=TRUE, col="red")
plot(GKolECDF, lty=1, lwd=1, verticals=TRUE, do.points=FALSE, add=TRUE, col="green")
plot(GSquaredECDF, lty=1, lwd=1, verticals=TRUE, do.points=FALSE, add=TRUE, col="brown")
abline(v=0, lty=5, col="grey")
x <- seq(min(Orig, GEKol), max(Orig, GEKol))
x0 <- x[which(abs(OrigECDF(x)-GEKolECDF(x))==max(abs(OrigECDF(x)-GEKolECDF(x))))]
y0 <- OrigECDF(x0)
y1 <- GEKolECDF(x0)
#par(mar=c(4.5,4.5,3,10), xpd=TRUE)
#legend("topright", lwd=2, lty=c(1,1,1,1,1), col=c("black","blue", "red", "green", "brown"), legend=c("Original","GE-Kolmogorov", "GE-Squared", "G-Kolmogorov", "G-Squared"), bty="n", inset = c(-0.4, 0.0))

ks.test(Orig[,1], GEKol[,1], alternative="two.sided") #Kolmogorow-Smirnow Test
