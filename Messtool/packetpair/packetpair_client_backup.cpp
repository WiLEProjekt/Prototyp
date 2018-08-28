/*
 * BACKUP FILE
 * CONTAINS A PACKET PAIR IMPLEMENTATION TO MEASURE THE BANDWIDTH
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <stdint.h>     //uint64_t
#include <inttypes.h>   //zum Printen von uint64_t
#include <time.h> //für die clock
#include <iostream>

using namespace std;

#define PAKETANZAHL 100 //Anzahl der zur RTT und Paketverlust gesendeten Pakete
#define ANZPAKETEMES 30000 //Anzahl der Pakete, die zur Bandbreitenmessung empfangen werden sollen
#define ANZMESSPUNKTE ANZPAKETEMES/100 //Anzahl der dokumentierten Messpunkte für die Bandbreitenmessung
#define UPLOADGROESSE 1450 //Paketgröße in Bytes für die Upload Messung mittels Packet Pairs



void error(char *err);
int receive(int sock, int paketNr, int lostPackets, int extra, uint64_t *start_pos);
uint64_t getTime();
uint64_t getTimens();
uint64_t calc_bandwidth(uint64_t byte, uint64_t timedff);
void wait(int msec);

int main(int argc, char** argv) {
    if(argc < 4){
        cout << "Usage: ./Client [Server-IP] [Server Port] [Number of Packet Pairs]" << endl;
        return 0;
    }
    string destinationIP = argv[1];
    int port = atoi(argv[2]);
    int maxpackets = atoi(argv[3]);
    //--------------------------------------------------------------------------
    //Socket Konfiguration
    //--------------------------------------------------------------------------
    int sock, bnd;
    sock = socket(AF_INET, SOCK_DGRAM, 0); //IPv4, UDP, standard Protokoll Feld im IP Header
    if (sock < 0) { //Falls beim socket öffnen ein Fehler auftritt
        return 0;
    }

    //Ziel Adresse
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    dest.sin_addr.s_addr = inet_addr(destinationIP.c_str()); //131.173.33.211

    //2 Sec Timeout auf receive function des Sockets setzen
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    int socktimeout = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    if(socktimeout<0){
        return 0;
    }

    //--------------------------------------------------------------------------
    //Bottleneck Link Bandwidth Messen (Upload)
    //--------------------------------------------------------------------------
    char rcvbandwidthMSG2[1400];
    struct sockaddr_in serverAddr;
    int serveraddrlen = sizeof(dest);
    char paket[UPLOADGROESSE];
    int pakete = 100;
    char str[3]; //Paketnummer

    for(int b = 0; b < sizeof(paket); b++){ //Paketnachricht Initialisieren
        paket[b] = 'a';
    }
    for(int i = 0; i<maxpackets; i++){
        sprintf(str,"%d", pakete+i); //Paketnummer in string umwandeln
        for(int f = 0; f<sizeof(str); f++){ //Paketnummer an Anfang der Paketnachricht schreiben
            paket[f] = str[f];
        }
        int sndp0 = sendto(sock, paket, UPLOADGROESSE, 0, (struct sockaddr*) &dest, sizeof (struct sockaddr_in)); //Paket senden
        if (sndp0 < 0) {
            return 0;
        }else{
            cout << i << endl;
        }
        if((i)%2 == 1){ //Nach dem senden eines Packet Pairs (2. Paket hat immer ungerade Paketnummer)
            wait(50);
        }
    }


    //--------------------------------------------------------------------------
    //Socket schließen
    //--------------------------------------------------------------------------
    int cls = close(sock);
    if (cls < 0) {
        return 0;
    }

    return 0;
}
//------------------------------------------------------------------------------
//Hilfsfunktionen
//------------------------------------------------------------------------------

//Gibt die aktuelle Zeit in ms zurück
uint64_t getTime(){
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time); //aktuelle Zeit erfassen
    uint64_t s = time.tv_sec;
    uint64_t ns = time.tv_nsec;
    uint64_t finalTime = s*1000000000+ns; //Zeit in Nanosekunden
    uint64_t finalTimems= finalTime/1000000; //Zeit in millisekunden
    return(finalTimems);
}

//Zeit in nanosekunden zurückgeben
uint64_t getTimens(){
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time); //aktuelle Zeit erfassen
    uint64_t s = time.tv_sec;
    uint64_t ns = time.tv_nsec;
    uint64_t finalTime = s*1000000000+ns; //Zeit in Nanosekunden
    return finalTime;
}

uint64_t calc_bandwidth(uint64_t byte, uint64_t timedff){
    if(timedff==0){ //wenn die Zwischenzeit kleiner 1ms ist, wird sie auf 1ms gesetzt um eine division durch 0 zu vermeiden. Kommt nur im localhost vor.
        timedff=1;
    }
    uint64_t byteProMs = byte/timedff; //Empfangene Byte pro ms
    uint64_t byteProS = byteProMs*1000; //Empfangene Byte pro sekunde
    uint64_t MBitProS = byteProS/125000; //Byte pro sekunde in Megabit pro sekunde umrechnen
    return(MBitProS);
}

void wait(int msec){
    uint64_t starttime = getTimens();
    while((getTimens()-starttime)/1000000 < msec);
}
