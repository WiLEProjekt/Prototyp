//
// Created by drieke on 22.06.18.
//

#include <sys/stat.h>
#include "Pingparser.h"


vector<bool> Pingparser::findMissingSeqNums(vector<unsigned int> sequenzNumbers) {
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
    string out = outputFile;
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

vector<bool> Pingparser::readPingFile(const string &filename, unsigned int packetNumber, string outputFile) {
    vector<bool> calculatedLosses = this->readPingFile(filename, packetNumber);
    if (!calculatedLosses.empty()) {
        this->wrtieTraceInFile(outputFile, calculatedLosses);
        return calculatedLosses;
    }
}

vector<bool> Pingparser::readPcapFile(const string &filename, Protocol protocol, string outputFile) {
    vector<bool> trace = this->readPcapFile(filename, protocol);
    if (!trace.empty()) {
        this->wrtieTraceInFile(outputFile, trace);
        return trace;
    }
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

string Pingparser::parseIP(unsigned char *bytes) {
    string ip;
    for (int i = 0; i < 4; i++) {
        unsigned char numberByte[] = {bytes[i]};
        unsigned int number = this->parseNumberFromBytes(numberByte, 1);
        ip += std::to_string(number);
        if (i < 4) {
            ip += ".";
        }
    }
    return ip;
}

vector<bool> Pingparser::findDuplicateSeqNums(const map<string, vector<unsigned int>> &seqNums) {
    vector<bool> packetloss;
    map<string, vector<unsigned int> > foundSeqNums;
    for (auto const &item : seqNums) {
        //Search for ip
        auto mapitem = foundSeqNums.find(item.first);
        if (mapitem != foundSeqNums.end()) {
            for (auto const &seqNum : mapitem->second) {
                if (std::find(item.second.begin(), item.second.end(), seqNum) != item.second.end()) {
                    //If the ip already had the seqNum, it is a packetloss, because of retransmision
                    packetloss.push_back(false);
                } else {
                    //If the ip has not the seqNum, it is a normal packet
                    packetloss.push_back(true);
                    vector<unsigned int> vec = mapitem->second;
                    vec.push_back(seqNum);
                    foundSeqNums.insert(mapitem, pair<string, vector<unsigned int>>(mapitem->first, vec));
                }
            }
        } else {
            //if the ip is not found, it was the first packet from that ip and no packeteloss
            packetloss.push_back(true);
            vector<unsigned int> newVector;
            foundSeqNums.insert(pair<string, vector<unsigned int>>(item.first, newVector));
        }
    }

    return packetloss;
}

vector<bool> Pingparser::readPingFile(const string &filename, unsigned int packetNumber) {
    fstream fileStream;
    struct stat buffer;
    if (stat(filename.c_str(), &buffer) != 0) {
        cout << "file " << filename << " not found" << endl;
    } else {
        fileStream.open(filename.c_str(), ios::in);
        vector<unsigned int> sequenzNumbers;

        for (string line; getline(fileStream, line);) { //Grab all lines
            if (line.find("icmp_seq=") != string::npos) { //search for the sequence number
                sequenzNumbers.push_back(parseSequenzNumberFromPing(line)); //parse string to int
            }
        }
        fileStream.close();
        vector<bool> calculatedLosses = findMissingSeqNums(sequenzNumbers);

        if (calculatedLosses.size() < packetNumber) { //detection if the last packages that were sent are lost
            int endLosses = packetNumber - calculatedLosses.size();
            for (int i = 0; i < endLosses; i++) {
                calculatedLosses.push_back(false);
            }
        }
        return calculatedLosses;
    }
}

vector<bool> Pingparser::readPcapFile(const string &filename, Protocol protocol) {
    struct stat buffer;
    if (stat(filename.c_str(), &buffer) != 0) {
        cout << "file " << filename << " not found" << endl;
    } else {
        vector<unsigned int> seqNums;
        char errbuff[PCAP_ERRBUF_SIZE];
        pcap_t *pcap = pcap_open_offline(filename.c_str(), errbuff);
        struct pcap_pkthdr *header;
        const u_char *data;
        map<string, vector<unsigned int>> tcpSeqNums;

        while (pcap_next_ex(pcap, &header, &data) >= 0) {
            if (header->len != header->caplen) {
                printf("Warning! Capture size different than packet size: %1d bytes\n", header->caplen);
            }

            //Wenn Bits 35 = 0 && 28 = 15 && 39 = 75, dann ICMP.
            //Wenn Bit 34 = 0 dann ICMP-Response
            switch (protocol) {
                case ICMP:
                    if (data[34] == 0 && data[35] == 0 && data[38] == 15 && data[39] == 75) {
                        unsigned char seqNumBytes[] = {data[40], data[41]};
                        seqNums.push_back(parseNumberFromBytes(seqNumBytes, 2));
                    }
                case TCP:
                    if (data[25] == 6) {
                        unsigned char seqNumBytes[] = {data[40], data[41], data[42], data[43]};
                        unsigned char sourceIPBytes[] = {data[26], data[27], data[28], data[29]};
                        unsigned char destIPBytes[] = {data[30], data[31], data[32], data[33]};
                        string sourceIp = this->parseIP(sourceIPBytes);
                        string destIp = this->parseIP(destIPBytes);
                        auto it = tcpSeqNums.find(sourceIp);
                        if (it != tcpSeqNums.end()) {
                            it->second.push_back(parseNumberFromBytes(seqNumBytes, 4));
                        } else {
                            vector<unsigned int> new_vector;
                            new_vector.push_back(parseNumberFromBytes(seqNumBytes, 4));
                            tcpSeqNums.insert(pair<string, vector<unsigned int>>(sourceIp, new_vector));
                        }
                    }
            }
        }
        vector<bool> trace;
        if (protocol == ICMP) {
            trace = findMissingSeqNums(seqNums);
        } else {
            trace = findDuplicateSeqNums(tcpSeqNums);
        }
        return trace;
    }
}
