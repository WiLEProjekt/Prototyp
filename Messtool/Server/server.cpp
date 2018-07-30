#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char **argv) {
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
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    int socktimeout = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (timeout));
    if (socktimeout < 0) {
        return 0;
    }

    int counter = 0;
    char rcvmsg[1000];
    struct sockaddr_in from;
    unsigned int flen = sizeof (struct sockaddr_in);
    while(true){
        int rcv = recvfrom(sock, rcvmsg, sizeof(rcvmsg), 0, (struct sockaddr*) &from, &flen); //Paket empfangen
        if (rcv < 0) { //Wenn 200sec gewartet wurden, ist die Paketverlustmessung garantiert vorbei
            break; //Dann Abbruch der Messung
        }else{
            counter++;
        }
    }
    cout << "pakete empfangen: " << counter << endl;
    return 0;
}


