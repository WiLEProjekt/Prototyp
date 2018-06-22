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

using namespace std;

class Pingparser {

private:
    const unsigned int MAX_SEQ_NUM = 65535;

    /**
     * Calculates the losses
     * @param sequenzNumbers the sequenzNumbers of the trace in order of their appearance
     * @return the calculated losses in order of their appearance
     */
    vector<bool> findLosses(vector<unsigned int> sequenzNumbers);

    /**
     * Writes the calculated losses in a file
     * @param outputFilename the filename
     * @param calculatedLossed the calculated lossed
     */
    void wrtieTraceInFile(const string &outputFile, vector<bool> calculatedLosses);

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

public:
    /**
    * Reads a ping file
    * @param filename the ping file
    * @param packetNumber the number of packets
    * @return the calculated losses
    */
    vector<bool> readPingFile(const string &filename, unsigned int packetNumber);

    /**
    * Reads a .pcap file
    * @param filename the .pcap filename
    * @return the calculated losses
    */
    vector<bool> readPcapFile(const string &filename);
};


#endif //TRACEGENERATOR_PINGPARSER_H
