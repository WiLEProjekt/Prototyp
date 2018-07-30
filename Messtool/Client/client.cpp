#include <iostream>
#include <sys/socket.h>
#include <string>
#include <arpa/inet.h>

using namespace std;

int main(int argc, char **argv) {
    if(argc < 2){
        cout << "zu wenige argumente" << endl;
        return 0;
    }
    string destinationIP = argv[1];
    int sock = socket(AF_INET, SOCK_DGRAM, 0); //IPv4, UDP, standard Protokoll Feld im IP Header
    if (sock < 0) { //Falls beim socket öffnen ein Fehler auftritt
        cout << "fehler beim Socket öffnen" << endl;
    }

    //Ziel Adresse
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(4722);
    dest.sin_addr.s_addr = inet_addr(destinationIP.c_str()); //131.173.33.211

    for(int i = 0; i< 10; i++){
        string paketverlustmsg = to_string(i);
        int snd = sendto(sock, paketverlustmsg.c_str(), paketverlustmsg.size(), 0, (struct sockaddr*) &dest, sizeof (struct sockaddr_in)); //Paket senden
        if (snd < 0) { //Wenn Sendefehler auftritt
            cout << "Sendefehler bei der RTT- & Paketverlustmessung" << endl; //Abbruch
            break;
        }
    }


    return 0;
}
