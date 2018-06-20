#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <pcap.h>
#include <cstring>
#include <map>
#include <stdlib.h>
#include <math.h>

using namespace std;
using std::ios;

const unsigned int MAX_SEQ_NUM = 65535;

/**
 * parses a sequenz number from a line of a ping
 * @param line the line of the ping
 * @return the sequenz number
 */
unsigned int parseSequenzNumberFromPing(string line) {
    string delimeterFront = "icmp_seq=";
    string delimeterBack = " ttl";
    size_t pos = line.find(delimeterFront);
    line.erase(0, pos + delimeterFront.length());
    pos = line.find(delimeterBack);
    string token = line.substr(0, pos);
    unsigned int seqNum;
    stringstream(token) >> seqNum;
    return seqNum;
}

/**
 * Parses a number from a byte array
 * @param the byte array
 * @param the length of the byte array
 * @return the parsed number
 */
unsigned int parseNumberFromBytes(unsigned char *bytes, int length) {
    unsigned int number = 0;
    for (int i = length - 1; i >= 0; i--) {
        number += bytes[length - i - 1] * pow(256, i);
    }

    return number;
}

/**
 * Writes the calculated losses in a file
 * @param outputFilename the filename
 * @param calculatedLossed the calculated lossed
 */
void writeTraceInFile(string outputFilename, vector<bool> calculatedLosses) {
    string out = "ParsedTraces/" + outputFilename;
    fstream file;
    file.open(out.c_str(), ios::out);
    unsigned int received = 0;
    for (int i = 0; i < calculatedLosses.size(); i++) {
        if (calculatedLosses[i]) {
            received++;
        }
        file << calculatedLosses[i];
    }
    file.close();
    cout << "trace successfully parsed" << endl;
    cout << "messages received: " << received << endl;
}

void calculateModelParameter(vector<bool> calculatedLosses) {

}

/**
 * Calculates the losses
 * @param sequenzNumbers the sequenzNumbers of the trace in order of their appearance
 * @return the calculated losses in order of their appearance
 */
vector<bool> findlosses(vector<unsigned int> sequenzNumbers) {
    vector<bool> calculatedLosses;
    unsigned int lastSeqNum = 0; //important initialization => detects if first few packages are lost
    unsigned long packetCounter = 0;
    bool firstSeqNumRed = false;

    for (unsigned long i = 0; i < sequenzNumbers.size(); i++) {
        unsigned int seqNum = sequenzNumbers.at(i);
        if (firstSeqNumRed) {
            int diff = seqNum - lastSeqNum;
            if (diff > 1) { //packets are lost in ascending order, maximum sequencenumber is not yet reached
                for (int i = 0; i < diff - 1; i++) { //Push lost packets
                    packetCounter++;
                    calculatedLosses.push_back(false);
                }
                calculatedLosses.push_back(true); //Push found packet
                packetCounter++;
            } else if (diff <= 0) { //Packet loss at maximum sequencenumber detected
                diff = MAX_SEQ_NUM - lastSeqNum + seqNum;
                for (int i = 0; i < diff; i++) {
                    packetCounter++;
                    calculatedLosses.push_back(false);
                }
                calculatedLosses.push_back(true); //push found packet
                packetCounter++;
            } else { //diff = 1 = no packet loss
                packetCounter++;
                calculatedLosses.push_back(true);
            }
        } else {
            calculatedLosses.push_back(true);
        }
        firstSeqNumRed = true;
        lastSeqNum = seqNum;
    }
    return calculatedLosses;
}

/**
 * Reads a .pcap file
 * @param filename the .pcap filename
 * @return the calculated losses
 */
vector<bool> readPcapFile(string filename) {
    vector<unsigned int> seqNums;
    char errbuff[PCAP_ERRBUF_SIZE];
    pcap_t *pcap = pcap_open_offline(filename.c_str(), errbuff);
    struct pcap_pkthdr *header;
    const u_char *data;
    u_int packetCount = 0;

    while (pcap_next_ex(pcap, &header, &data) >= 0) {
        if (header->len != header->caplen) {
            printf("Warning! Capture size different than packet size: %1d bytes\n", header->caplen);
        }

        //Wenn Bits 35 = 0 && 28 = 15 && 39 = 75, dann ICMP.
        //Wenn Bit 34 = 0 dann ICMP-Response
        if (data[34] == 0 && data[35] == 0 && data[38] == 15 && data[39] == 75) {
            unsigned char seqNumBytes[] = {data[40], data[41]};
            seqNums.push_back(parseNumberFromBytes(seqNumBytes, 2));
        } else {
            unsigned char seqNumBytes[] = {data[40], data[41], data[42], data[43]};
            unsigned char ackNumBytes[] = {data[44], data[45], data[46], data[47]};
        }
        for (u_int i = 0; i < header->len; i++) {
            if (i % 16 == 0) {
                printf("\n");
            }
            printf("%.2x ", data[i]);
        }
        printf("\n\n");
    }

    return findlosses(seqNums);
}

/**
 * Reads a ping file
 * @param filename the ping file
 * @param packetNumber the number of packets
 * @return the calculated losses
 */
vector<bool> readPingFile(string filename, unsigned int packetNumber) {
    fstream fileStream;
    fileStream.open(filename.c_str(), ios::in);
    vector<unsigned int> sequenzNumbers;

    for (string line; getline(fileStream, line);) { //Grab all lines
        if (line.find("icmp_seq=") != string::npos) { //search for the sequence number
            sequenzNumbers.push_back(parseSequenzNumberFromPing(line)); //parse string to int
        }
    }
    fileStream.close();
    vector<bool> calculatedLosses = findlosses(sequenzNumbers);

    if (calculatedLosses.size() < packetNumber) { //detection if the last packages that were sent are lost
        int endLosses = packetNumber - calculatedLosses.size();
        for (int i = 0; i < endLosses; i++) {
            calculatedLosses.push_back(false);
        }
    }
    return calculatedLosses;
}


/**
 * Prints the correct call of the Program
 */
void printArgError() {
    cout << "Pingparser -ping [filename of pingtrace] [output-filename .txt] "
         << "[total number of packets used in the pingtrace]\n"
         << "Pingparser -pcap [filename of trace .pcap] [output-filename .txt]" << endl;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        printArgError();
        return -1;
    }

    string inputFilename = argv[2];
    string outputFilename = argv[3];
    vector<bool> calculatedLossed;

    if (strcmp(argv[1], "-ping") == 0) {
        unsigned int packetNumber = atoi(argv[4]);
        calculatedLossed = readPingFile(inputFilename, packetNumber);
    } else if (strcmp(argv[1], "-pcap") == 0) {
        calculatedLossed = readPcapFile(inputFilename);
    }
    writeTraceInFile(outputFilename, calculatedLossed);

    return 0;
}