#include <iostream>
#include <sys/socket.h>
#include <string>
#include <arpa/inet.h>
#include "PcapWriter.h"
#include <pcap.h>
#include <strings.h>
#include <thread>
#include <sys/stat.h>

using namespace std;

pcap_t *handle;

int writePcap(char * dev, char* outputFilename) {
    char errbuf[PCAP_ERRBUF_SIZE];	/* Error string */
    struct bpf_program fp{};		/* The compiled filter */
    char filter_exp[] = "port 23";	/* The filter expression */
    bpf_u_int32 mask;		/* Our netmask */
    bpf_u_int32 net;		/* Our IP */
    struct pcap_pkthdr header{};	/* The header that pcap gives us */
    const u_char *packet;		/* The actual packet */
    pcap_dumper_t *pcapFile;
    struct pcap_stat stat{}; /* Some pcap stats */

    if (dev == nullptr) {
        fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
        return(2);
    }
    /* Find the properties for the device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
        net = 0;
        mask = 0;
    }
    /* Open the session in promiscuous mode */
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == nullptr) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return(2);
    }

    pcap_setnonblock(handle, 0, errbuf);

    if ((pcapFile = pcap_dump_open(handle, outputFilename)) == nullptr) {
        fprintf(stderr, "Error from pcap_dump_open(): %s\n", pcap_geterr(handle));
        exit(1);
    }

    if ((pcap_dispatch(handle, -1, pcap_dump, (u_char *)pcapFile)) != 0) {
        fprintf(stderr, "Error from pcap_loop(): %s\n", pcap_geterr(handle));
        exit(1);
    }
    cout << "tut noch nach loop" << endl;

    if ((pcap_stats(handle, &stat) != 0)) {
        fprintf(stderr, "Error from pcap_stats(): %s\n", pcap_geterr(handle));
        exit(1);
    }
    else {
        printf("Statistik from pcap\n");
        printf(" packets received  : %d\n", stat.ps_recv);
        printf(" packets dropped   : %d\n", stat.ps_drop);
        printf(" drops by interface: %d\n", stat.ps_ifdrop);
    }


    pcap_dump_close(pcapFile);
    pcap_close(handle);
    return(0);
}

void stopPcap(){
    pcap_breakloop(handle);
}

void startSending(string destinationIP, int sock, PcapWriter *writer){
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
    writer->stop();
}


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

    PcapWriter *writer = new PcapWriter();
    writer->start("enp0s31f6", "test.pcap");
    cout << "pcap started" << endl;

    startSending(destinationIP, sock, writer);

    return 0;
}