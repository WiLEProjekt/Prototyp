//
// Created by drieke on 30.07.18.
//

#include "PcapWriter.h"

int PcapWriter::writePcap(char * dev, char* outputFilename) {
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

    if ((pcap_loop(handle, 100, pcap_dump, (u_char *)pcapFile)) != 0) {
        fprintf(stderr, "Error from pcap_loop(): %s\n", pcap_geterr(handle));
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

void PcapWriter::start(char *dev, char* outputFilename) {
    /*thread pcapThread(writePcap, dev, outputFilename);
    pcapThread.join();*/
    this->writePcap(dev, outputFilename);
    cout << "Pcap started" << endl;
}

void PcapWriter::stop() {
    pcap_breakloop(handle);
}
