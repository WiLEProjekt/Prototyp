#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <pcap.h>
#include <cstring>
#include <map>
#include <stdlib.h>

using namespace std;
using std::ios;

const unsigned int MAX_SEQ_NUM = 65535;


unsigned int parseSeqNumFromHex(unsigned char first, unsigned char second) {
    unsigned int seqNum = first * (unsigned int) 256 + second;
    printf("seqNum: %u\n", seqNum);
    return seqNum;
}

void readPcapFile(char *filename, const string &outputFilename) {
    vector<unsigned int> seqNums;
    printf("File: %s", filename);
    char errbuff[PCAP_ERRBUF_SIZE];
    pcap_t *pcap = pcap_open_offline(filename, errbuff);
    struct pcap_pkthdr *header;
    const u_char *data;
    u_int packetCount = 0;
    while (pcap_next_ex(pcap, &header, &data) >= 0) {
        printf("Packet # %i\n", ++packetCount);
        printf("Packet size: %d bytes\n", header->len);
        if (header->len != header->caplen) {
            printf("Warning! Capture size different than packet size: %1d bytes\n", header->caplen);
        }
        printf("Epoch Time: %li:%li seconds\n", header->ts.tv_sec, header->ts.tv_usec);

        printf("Data: %.2x %.2x<\n%d %d\n", data[38], data[39], data[38], data[39]);
        if (data[34] == 0 && data[35] == 0 && data[38] == 15 &&
            data[39] == 75) { //35 = 0 && 28 = 15 & 39 = 75: ICMP, 34 = 0: response
            printf("true\n");
            seqNums.push_back(parseSeqNumFromHex(data[40], data[41]));
        } else {
            printf("false: %d %d %d %d", data[35], data[36], data[38], data[39]);
        }
        for (u_int i = 0; i < header->len; i++) {
            if (i % 16 == 0) {
                printf("\n");
            }
            printf("%.2x ", data[i]);
        }
        printf("\n\n");
    }

    unsigned int lastSeqNum = 0;
    unsigned long packetCounter = 0;
    vector<bool> parsedTrace;
    bool firstSeqNumRed = false;

    for (unsigned long i = 0; i < seqNums.size(); i++) {
        unsigned int seqNum = seqNums.at(i);
        if (firstSeqNumRed) {
            int diff = seqNum - lastSeqNum;
            if (diff > 1) { //packets are lost in ascending order, maximum sequencenumber is not yet reached
                for (int i = 0; i < diff - 1; i++) { //Push lost packets
                    packetCounter++;
                    parsedTrace.push_back(false);
                }
                parsedTrace.push_back(true); //Push found packet
                packetCounter++;
            } else if (diff <= 0) { //Packet loss at maximum sequencenumber detected
                diff = MAX_SEQ_NUM - lastSeqNum + seqNum;
                for (int i = 0; i < diff; i++) {
                    packetCounter++;
                    parsedTrace.push_back(false);
                }
                parsedTrace.push_back(true); //push found packet
                packetCounter++;
            } else { //diff = 1 = no packet loss
                packetCounter++;
                parsedTrace.push_back(true);
            }
        }
        firstSeqNumRed = true;
        lastSeqNum = seqNum;
        cout << "seqNum: " << seqNum << endl;
    }

    string out = "../ParsedTraces/" + outputFilename;
    fstream file;
    file.open(out.c_str(), ios::out);
    unsigned int received = 0;
    for (int i = 0; i < parsedTrace.size(); i++) {
        if (parsedTrace[i]) {
            received++;
        }
        file << parsedTrace[i];
    }
    file.close();
    cout << "trace successfully parsed" << endl;
    cout << "messages received: " << received << endl;
}

unsigned int parseSequenzNumber(string line) {
    ;
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

unsigned long parsePacketNumber(string line) {
    string delimeter = " packets transmitted, ";
    string token = line.substr(0, line.find(delimeter));
    unsigned long packetNumberValue;
    stringstream(token) >> packetNumberValue;
    return packetNumberValue;
}

void printArgError() {
    cout
            << "Pingparser -ping [filename of pingtrace] [output-filename .txt] [total number of packets used in the pingtrace]\nPingparser -pcap [filename of trace .pcap] [output-filename .txt]"
            << endl;
}

int main(int argc, char **argv) {
    unsigned long packetCounter = 0;

    vector<bool> parsedTrace;
    if (argc < 4) {
        printArgError();
        return -1;
    }
    if (strcmp(argv[1], "-ping") == 0) {
        string inputFilename = argv[1];
        string outputFilename = argv[2];
        unsigned int packetNumber = atoi(argv[3]);

        fstream fileStream;
        fileStream.open(inputFilename.c_str(), ios::in);
        unsigned int lastSeqNum = 0; //important initialization => detects if first few packages are lost

        for (string line; getline(fileStream, line);) { //Grab all lines
            if (line.find("icmp_seq=") != string::npos) { //search for the sequence number
                unsigned int seqNum = parseSequenzNumber(line); //parse string to int
                int diff = seqNum - lastSeqNum;
                if (diff > 1) { //packets are lost in ascending order, maximum sequencenumber is not yet reached
                    for (int i = 0; i < diff - 1; i++) { //Push lost packets
                        packetCounter++;
                        parsedTrace.push_back(false);
                    }
                    parsedTrace.push_back(true); //Push found packet
                    packetCounter++;
                } else if (diff <= 0) { //Packet loss at maximum sequencenumber detected
                    diff = MAX_SEQ_NUM - lastSeqNum + seqNum;
                    for (int i = 0; i < diff; i++) {
                        packetCounter++;
                        parsedTrace.push_back(false);
                    }
                    parsedTrace.push_back(true); //push found packet
                    packetCounter++;
                } else { //diff = 1 = no packet loss
                    packetCounter++;
                    parsedTrace.push_back(true);
                }

                lastSeqNum = seqNum;
            }
        }
        fileStream.close();
        if (packetCounter < packetNumber) { //detection if the last packages that were sent are lost
            for (int i = 0; i < packetNumber - 1; i++) {
                parsedTrace.push_back(false);
            }
        }
        string out = "../ParsedTraces/" + outputFilename;
        fstream file;
        file.open(out.c_str(), ios::out);
        unsigned int received = 0;
        for (int i = 0; i < parsedTrace.size(); i++) {
            if (parsedTrace[i]) {
                received++;
            }
            file << parsedTrace[i];
        }
        file.close();
        cout << "trace successfully parsed" << endl;
        cout << "messages received: " << received << endl;
    } else if (strcmp(argv[1], "-pcap") == 0) {
        readPcapFile(argv[2], argv[3]);
    }

    return 0;
}