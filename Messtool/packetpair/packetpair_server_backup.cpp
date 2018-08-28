#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <stdint.h>     //uint64_t
#include <inttypes.h>   //zum Printen von uint64_t
#include <time.h> //für die clock

#define PACKETSZ 1400 //Packetgröße des Packets zur Bandbreitenmessung
#define UPLOADMAXDIGITS 8//Maximale Größe der übertragbaren Uploadgeschwindigkeit
#define PPSAMPLES 5 //Anzahl der benötigten Packet Pairs zur Uploadmessung

uint64_t getTimens() { //Zeit in Nanosekunden wiedergeben
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time); //aktuelle Zeit erfassen
    uint64_t s = time.tv_sec;
    uint64_t ns = time.tv_nsec;
    uint64_t finalTime = s * 1000000000 + ns;
    return finalTime;
}

int main(int argc, char** argv) {

    //--------------------------------------------------------------------------
    //Socket Konfiguration
    //--------------------------------------------------------------------------

    //Socket öffnen
    int sock = socket(AF_INET, SOCK_DGRAM, 0); //IPv4, UDP, standard Protokoll Feld im IP Header
    if (sock < 0) { //Falls beim socket öffnen ein Fehler auftritt
        return 0;
    }

    //Eigene Adresse
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4722);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //Eigene Adresse an socket binden
    int bnd = bind(sock, (struct sockaddr *) &addr, sizeof (struct sockaddr_in));
    if (bnd < 0) {
        return 0;
    }

    //Timeout auf receive function des Sockets setzen
    struct timeval timeout;
    timeout.tv_sec = 200;
    timeout.tv_usec = 0;
    int socktimeout = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (timeout));
    if (socktimeout < 0) {
        return 0;
    }

    //--------------------------------------------------------------------------
    //Paketverlust & RTT messen
    //--------------------------------------------------------------------------
    char rcvmsg[30000];
    struct sockaddr_in from;
    unsigned int flen = sizeof (struct sockaddr_in);


    //--------------------------------------------------------------------------
    //Bottleneck Link Messen mittels Packet Pairs
    //--------------------------------------------------------------------------
    //Neues Timeout setzen
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    int socktimeout2 = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (timeout));
    if (socktimeout2 < 0) {
        return 0;
    }

    uint64_t rcvtimes[999] = {0};
    int aufeinanderfolgend = 0;
    int nbr, size, abbruch = 0;
    char number[4];
    for(int i = 0; i<sizeof(number); i++){ //initialisierung des Arrays
        number[i] = ' ';
    }
    while (1) {
        int rcv0 = recvfrom(sock, rcvmsg, sizeof (rcvmsg), 0, (struct sockaddr*) &from, &flen);
        if (rcv0 < 0) { //Wenn 20sec lang garkein Paket ankommt, schlägt die Messung fehl, weil der Paketverlust zu hoch ist.
            abbruch = 1;
            break;
        }
        strncpy(number, rcvmsg, 3); //Die ersten 3 Zeichen der empfangenen Nachricht bilden die Paket nummer
        nbr = atoi(number); //Paketnummer in Zahl umwandeln
        //printf("%i \n", nbr);
        size = rcv0; //Größe der empfangenen Nachricht zwischenspeichern
        rcvtimes[nbr - 100] = getTimens(); //Empfangszeit der Nachricht in Array speichern. Arraypos. 0 gehört zu Paket 100, 1 zu 101,...
        if (nbr % 2 == 1 && rcvtimes[nbr - 101] != 0) { //packet pair erkennen
            aufeinanderfolgend++; //Counter für empfangene Packet Pairs inkrementieren
        }
    }
    //Times berechnen
    int roundedBandwidth = 0;
    int loop = 0, counter = 0;
    uint64_t times = 0;
    while (loop < 998) { //über die einträge des Arrays mit den Empfangszeiten iterieren
        if (rcvtimes[loop] != 0 && rcvtimes[loop + 1] != 0) { //Prüfen ob Packet Pair gefunden ist
            times = rcvtimes[loop + 1] - rcvtimes[loop]; //Differenzen der Receivetimes aufaddieren
            long double dtimes = (long double) times / 1000000000; //Nanosekunden in Sekunden umrechnen
            long double sizembit = (long double) size / 125000; //Bytes in Mbit umrechnen
            long double bandwidth = (sizembit) / dtimes; //Bottleneck Link Bandbreite berechnen
            roundedBandwidth = (int) (bandwidth + 0.5); //auf ganze Zahl runden, um Übertragung zum Client zu vereinfachen
            printf("%i \n", roundedBandwidth);
            loop = loop + 2;
        } else {
            loop = loop + 2;
        }
    }

    //Socket schließen
    int cls = close(sock);
    if (cls < 0) {
        return 0;
    }
}