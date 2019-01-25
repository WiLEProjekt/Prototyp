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
#include "Result.h"
#include "SignalStrength.h"
#include "PcapValues.h"
#include "ResultPoint.h"

using namespace std;

class PcapParser {

    unsigned int parseNumberFromBytes(const unsigned char *bytes, int length);

    string parseIP(unsigned char *bytes);

    unsigned long getSeqNumIperf(const u_char *data);

    unsigned long getSeqNumMobileTool(const u_char *data);

    uint64_t getMillisFromTimeval(struct timeval tv);


    /**
    * Calculates the Delay, Loss, Reordering and Duplication
    * @param values the values from the pcaps
    * @return the resutls
    */
    struct result getResults(struct pcapValues values);

    struct pcapValues
    readMobilePcapFile(const string &filename, const string &ipOfPcapDevice, const string &ipOfOtherDevice);

    struct pcapValues readPcapFile(const string &filename, const string &ipOfPcapDevice, const string &ipOfOtherDevice);

    void writeDelayFile(const string &filename, vector<int64_t> delays);

    void writeFullTraceFile(const string &filename, vector<resultPoint> result);

    void writeResultToFile(const result &download);


    struct result
    startParsing(const string &clientTraceFile, const string &serverTraceFile, const string &globalClientIp) {
        this->startParsing(clientTraceFile, serverTraceFile, globalClientIp, "192.168.8.100", "131.173.33.228");
    }

    struct result
    startParsing(const string &clientTraceFile, const string &serverTraceFile, const string &globalClientIp,
                 const string &localClientIp, const string &serverIp) {
        this->startParsing(clientTraceFile, serverTraceFile, globalClientIp, localClientIp, serverIp, false);
    }

    struct result
    startParsing(const string &clientTraceFile, const string &serverTraceFile, const string &globalClientIp,
                 const string &localClientIp, const string &serverIp, bool parameterized);

    int main(int argc, char **argv);
};


#endif //TRACEGENERATOR_PCAPPARSER_H
