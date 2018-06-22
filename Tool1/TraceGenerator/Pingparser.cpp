//
// Created by drieke on 22.06.18.
//

#include "Pingparser.h"

vector<bool> Pingparser::findLosses(vector<unsigned int> sequenzNumbers) {
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

void Pingparser::wrtieTraceInFile(const string &outputFile, vector<bool> calculatedLosses) {
    string out = "ParsedTraces/" + outputFile;
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

unsigned int Pingparser::parseNumberFromBytes(unsigned char *bytes, int length) {
    unsigned int number = 0;
    for (int i = length - 1; i >= 0; i--) {
        number += bytes[length - i - 1] * pow(256, i);
    }

    return number;
}

vector<bool> Pingparser::readPingFile(const string &filename, unsigned int packetNumber) {
    fstream fileStream;
    fileStream.open(filename.c_str(), ios::in);
    vector<unsigned int> sequenzNumbers;

    for (string line; getline(fileStream, line);) { //Grab all lines
        if (line.find("icmp_seq=") != string::npos) { //search for the sequence number
            sequenzNumbers.push_back(parseSequenzNumberFromPing(line)); //parse string to int
        }
    }
    fileStream.close();
    vector<bool> calculatedLosses = findLosses(sequenzNumbers);

    if (calculatedLosses.size() < packetNumber) { //detection if the last packages that were sent are lost
        int endLosses = packetNumber - calculatedLosses.size();
        for (int i = 0; i < endLosses; i++) {
            calculatedLosses.push_back(false);
        }
    }
    return calculatedLosses;
}

vector<bool> Pingparser::readPcapFile(const string &filename) {
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

    return findLosses(seqNums);
}

unsigned int Pingparser::parseSequenzNumberFromPing(string line) {
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
