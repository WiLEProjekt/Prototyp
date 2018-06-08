Netem = read.table("Dokumente/Prototyp/Tool1/Validationtool/build/GilbertElliotNetem.txt")
Tool = read.table("Dokumente/Prototyp/Tool1/Validationtool/build/GilbertElliotTool.txt")
yGEN = Netem[,1]
yGET = Tool[,1]
FnGEN <- ecdf(yGEN)
FnGET <- ecdf(yGET)
x <- c(seq(-10,60,by=10))
plot(FnGEN, lty=2, lwd=3, verticals=TRUE, do.points=FALSE, col="red", xlim=c(-10,60), main="ECDF of Gilbert-Elliot with p=20%, r=70%, 1-k=5%, 1-h=95%", xlab="Burstsize [Number of Packets] (Negative = Loss, Positive = No Loss)", ylab="Cumulative Probability")
plot(FnGET, lty=3, lwd=3, verticals=TRUE, do.points=FALSE, add=TRUE, col="blue")
legend("bottomright", lwd=2, lty=c(2,3), col=c("red","blue"), legend=c("Netem","Our Tool"), bty="n")
abline(v=0, lty=3)
ks.test(yGEN, yGET, alternative="two.sided")