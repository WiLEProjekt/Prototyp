distancesB <- read.table("Dokumente/FinaleLossAuswertung/29012019/OS_to_MS2/Auswertung_Rohdaten/BernoulliKolmogorovDistances.txt")
distancesSG <- read.table("Dokumente/FinaleLossAuswertung/29012019/OS_to_MS2/Auswertung_Rohdaten/SimpleGilbertKolmogorovDistances.txt")
distancesG <- read.table("Dokumente/FinaleLossAuswertung/29012019/OS_to_MS2/Auswertung_Rohdaten/GilbertKolmogorovDistances.txt")
distancesGE <- read.table("Dokumente/FinaleLossAuswertung/29012019/OS_to_MS2/Auswertung_Rohdaten/GilbertElliotKolmogorovDistances.txt")
distancesM <- read.table("Dokumente/FinaleLossAuswertung/29012019/OS_to_MS2/Auswertung_Rohdaten/MarkovKolmogorovDistances.txt")

library(ggplot2)

distancesB_2 <- data.frame(distancesB, "Model")
distancesSG_2 <- data.frame(distancesSG, "Model")
distancesG_2 <- data.frame(distancesG, "Model")
distancesGE_2 <- data.frame(distancesGE, "Model")
distancesM_2 <- data.frame(distancesM, "Model")

distancesB_2$X.Model. <- "Bernoulli"
distancesSG_2$X.Model. <- "Simple-Gilbert"
distancesG_2$X.Model. <- "Gilbert"
distancesGE_2$X.Model. <- "Gilbert-Elliot"
distancesM_2$X.Model. <- "Markov"

#distance_final <- rbind.data.frame(distancesB_2, distancesSG_2, distancesG_2, distancesGE_2)
distance_final <- rbind.data.frame(distancesB_2, distancesSG_2, distancesG_2, distancesGE_2, distancesM_2)

ggplot(data = distance_final, aes(x=X.Model., y=V1)) + 
  geom_boxplot(aes(fill=X.Model.), notch = TRUE) + xlab("Models") + ylab("Kolmogorov distance") + theme_bw() + theme(legend.position="none") + ggtitle("Train Osnabrück-Münster-2 29.01.2019")