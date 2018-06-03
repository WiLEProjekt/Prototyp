GilbertElliotNetem = read.table("Documents/Prototyp/Tool1/Validationtool/build/GilbertElliotVal.txt")
y = GilbertElliotNetem[,1]
x = 1:65771
plot(x,y, type="l", xlab="Burstnumber", ylab="Burstsize [number of packets]", main="Gilbert-Elliot Netem p=20%, r=70% 1-k=5% 1-h=95%")