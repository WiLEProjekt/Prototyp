//
// Created by drieke on 22.06.18.
//

#ifndef TRACEGENERATOR_PINGPARSER_H
#define TRACEGENERATOR_PINGPARSER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <math.h>
#include <pcap.h>
#include <sstream>
#include <algorithm>
#include <map>

using namespace std;

enum Protocol {
    ICMP,
    TCP,
    NONE
};


class Pingparser {

private:
    const unsigned int MAX_SEQ_NUM = 65535;

    /**
     * Calculates the losses
     * @param sequenzNumbers the sequenzNumbers of the trace in order of their appearance
     * @return the calculated losses in order of their appearance
     */
    vector<bool> findMissingSeqNums(vector<unsigned int> sequenzNumbers);

    /**
     * Writes the calculated losses in a file
     * @param outputFilename the filename
     * @param calculatedLossed the calculated lossed
     */
    void writeTraceInFile(const string &outputFile, vector<bool> calculatedLosses);

    /**
     * Parses a number from a byte array
     * @param the byte array
     * @param the length of the byte array
     * @return the parsed number
     */
    unsigned int parseNumberFromBytes(unsigned char *bytes, int length);

    /**
     * parses a sequenz number from a line of a ping
     * @param line the line of the ping
     * @return the sequenz number
     */
    unsigned int parseSequenzNumberFromPing(string line);

    /**
     * Finds duplicate seqNums in to detect packetloss in tcp
     * @param seqNums a map of the source ips and the seqNums
     * @return a 01-trace
     */
    vector<bool> findDuplicateSeqNums(const map<string, vector<unsigned int>> &seqNums);

    /**
     * Parses an IP-Address from bytes
     * @param bytes the bytes
     * @return the IP-Address
     */
    string parseIP(unsigned char bytes[4]);

public:
    /**
    * Reads a ping file
    * @param filename the ping file
    * @param packetNumber the number of packets
    * @param outputFile the output file
    * @return the calculated losses
    */
    vector<bool> readPingFile(const string &filename, unsigned int packetNumber, string outputFile);

    vector<bool> readPingFile(const string &filename, unsigned int packetNumber);

    /**
    * Reads a .pcap file
    * @param filename the .pcap filename
    * @param outputFile the output file
    * @return the calculated losses
    */
    vector<bool> readPcapFile(const string &filename, Protocol protocol, string outputFile);

    vector<bool> readPcapFile(const string &filename, Protocol protocol);

    /**
     * Reads a trace in a 01-file
     * @param filename the file
     * @return the trace
     */
    vector<bool> readFile(string filename);
};


#endif //TRACEGENERATOR_PINGPARSER_H
