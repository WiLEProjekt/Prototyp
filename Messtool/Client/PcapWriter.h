//
// Created by drieke on 30.07.18.
//

#ifndef PCAPWRITER_PCAPWRITER_H
#define PCAPWRITER_PCAPWRITER_H

#include <iostream>
#include <pcap.h>
#include <strings.h>
#include <thread>
#include <sys/stat.h>

using namespace std;

class PcapWriter {
private:
    pcap_t *handle;			/* Session handle */
    int writePcap(char * dev, char* outputFilename);

public:
    void start(char * dev, char* outputFilename);
    void stop();
};



#endif //PCAPWRITER_PCAPWRITER_H
