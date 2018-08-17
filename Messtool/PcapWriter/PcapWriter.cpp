//
// Created by drieke on 30.07.18.
//

#include <csignal>
#include <unistd.h>
#include "PcapWriter.h"

pcap_t *handle; /* Session handle */

void terminate_process(int signum){
    pcap_breakloop(handle);
    pcap_close(handle);
}

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


    if ((pcapFile = pcap_dump_open(handle, outputFilename)) == nullptr) {
        fprintf(stderr, "Error from pcap_dump_open(): %s\n", pcap_geterr(handle));
        exit(1);
    }

    signal(SIGINT, terminate_process);
    if ((pcap_loop(handle, -1, pcap_dump, (u_char *)pcapFile)) != 0) {
        fprintf(stderr, "pcap_loop() canceled: %s\n", pcap_geterr(handle));
        exit(1);
    }

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

void secondThread(){
    for(int i = 0; i < 10; i++) {
        sleep(5);
        cout << "jyp" << endl;
    }
}

void PcapWriter::start(char *dev, char* outputFilename) {
    printf("PcapWriter started successfully\n");
    thread pcapThread(writePcap, dev, outputFilename);
    thread secondThreadThread(secondThread);
    secondThreadThread.join();
    pcapThread.join();
    //this->writePcap(dev, outputFilename);
}

/*
int main(int argc, char **argv) {
    PcapWriter *writer = new PcapWriter();
    writer->start("enp0s3", "test.pcap");
    cout << "lebt noch" << endl;
}*/
