//
// Created by drieke on 25.01.19.
//

#ifndef TRACEGENERATOR_PCAPPARSER_H
#define TRACEGENERATOR_PCAPPARSER_H

#include <iostream>
#include <pcap.h>
#include <sys/stat.h>
#include <cmath>
#include <utility>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
#include <string>
#include <dirent.h>
#include <experimental/filesystem>
#include <set>
#include <sstream>
#include <iterator>
#include <cstdint>
#include <stdint-gcc.h>
#include "Result.h"
#include "PcapValues.h"
#include "ResultPoint.h"

using namespace std;

class PcapParser {
private:
    unsigned int parseNumberFromBytes(const unsigned char *bytes, int length);

    string parseIP(unsigned char *bytes);

    unsigned long getSeqNumMobileTool(const u_char *data);

    uint64_t getMillisFromTimeval(struct timeval tv);

    /**
    * Calculates the Delay, Loss, Reordering and Duplication
    * @param values the values from the pcaps
    * @return the resutls
    */
    struct result *getResults(struct pcapValues *values);

    struct pcapValues *
    readMobilePcapFile(const string &filename, const string &ipOfPcapDevice, const string &ipOfOtherDevice);

public:
    struct result
    *startParsing(const string &clientTraceFile, const string &serverTraceFile, const string &globalClientIp) {
        return this->startParsing(clientTraceFile, serverTraceFile, globalClientIp, "192.168.8.100", "131.173.33.228");
    }

    struct result
    *startParsing(const string &clientTraceFile, const string &serverTraceFile, const string &globalClientIp,
                  const string &localClientIp, const string &serverIp);
};


#endif //TRACEGENERATOR_PCAPPARSER_H
