#include <iostream>
#include <sys/socket.h>
#include <string>
#include <arpa/inet.h>
#include <time.h>

using namespace std;

void wait(int msec){ //wait for x milliseconds
    clock_t goaltime = msec+clock();
    while(goaltime>clock());
}

int main(int argc, char **argv) {
    if(argc < 4){
        cout << "Usage: ./Client [Server-IP] [Server Port] [Number of Packet Pairs]" << endl;
        return 0;
    }
    string destinationIP = argv[1];
    int port = atoi(argv[2]);
    int maxpackets = atoi(argv[3]);
    int sock = socket(AF_INET, SOCK_DGRAM, 0); //IPv4, UDP, standard Protocoll Field in IP Header
    if (sock < 0) {
        cout << "Error while opening the Socket" << endl;
        return 0;
    }

    //Destination Address
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    dest.sin_addr.s_addr = inet_addr(destinationIP.c_str());


    //Upload Test using Packet Pair
    string message = "";
    for(int i = 0; i<1450; i++){ //initialize message with dummy payload
        message +="a";
    }
    for(int i = 0; i< maxpackets; i++){
        string number = to_string(i);
        for(int b = 0; b<number.size(); b++){
            message.at(b) = number.at(b);
        }
        cout << message.size() << endl;
        int snd = sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr*) &dest, sizeof (struct sockaddr_in)); //Paket senden
        if (snd < 0) { //Wenn Sendefehler auftritt
            cout << "Sendefehler" << endl; //Abbruch
            break;
        }
        if((i%2) == 1){ //Wait for some time when a packet pair is sent to avoid buffer overflow
            wait(50);
        }
    }


    return 0;
}
