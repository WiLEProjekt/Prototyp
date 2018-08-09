#include <iostream>
#include <pcap.h>
#include <sys/stat.h>
#include <cmath>
#include <vector>
#include <map>

using namespace std;

const unsigned int MAX_SEQ_NUM = 65535;

unsigned int parseNumberFromBytes(const unsigned char *bytes, int length) {
    unsigned int number = 0;
    for (int i = length - 1; i >= 0; i--) {
        number += bytes[length - i - 1] * pow(256, i);
    }

    return number;
}

string parseIP(unsigned char *bytes) {
    string ip;
    for (int i = 0; i < 4; i++) {
        unsigned char numberByte[] = {bytes[i]};
        unsigned int number = parseNumberFromBytes(numberByte, 1);
        ip += std::to_string(number);
        if (i < 4) {
            ip += ".";
        }
    }
    return ip;
}

int getSeqNum(const u_char *data) {
    //TODO
    return -1;
}

struct timeval findResponse(int seqNum, const map<int, struct timeval> &responseTimestamps) {
    //TODO
    return nullptr;
}

vector<struct timeval>
getDelays(map<int, struct timeval> requestTimestamps, const map<int, struct timeval> &responseTimestamps) {
    vector<struct timeval> delays;
    for (const auto &iterator : requestTimestamps) {
        struct timeval delay{};
        struct timeval responseTimestamp = findResponse(iterator.first, responseTimestamps);
        delay.tv_sec = (responseTimestamp.tv_sec - iterator.second.tv_sec) / 2;
        delay.tv_usec = (responseTimestamp.tv_usec - iterator.second.tv_usec) / 2;
        delays.push_back(delay);
    }

    return delays;
}

vector<bool> findMissingSeqNums(vector<unsigned int> sequenzNumbers) {
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

vector<bool> getLoss(vector<unsigned int> seqNums) {
    vector<bool> trace;
    trace = findMissingSeqNums(seqNums);
}

vector<struct timeval> readPcapFile(const string &filename, const string &sourceIp, const string &destIp) {
    map<int, struct timeval> requestTimestamps;
    map<int, struct timeval> responseTimestamps;
    vector<unsigned int> seqNums;

    struct stat buffer{};
    if (stat(filename.c_str(), &buffer) != 0) {
        cout << "file " << filename << " not found" << endl;
    } else {
        char errbuff[PCAP_ERRBUF_SIZE];
        pcap_t *pcap = pcap_open_offline(filename.c_str(), errbuff);
        struct pcap_pkthdr *header;
        const u_char *data;

        while (pcap_next_ex(pcap, &header, &data) >= 0) {
            if (header->len != header->caplen) {
                printf("Warning! Capture size different than packet size: %1d bytes\n", header->caplen);
            }

            unsigned char sourceIPBytes[] = {data[26], data[27], data[28], data[29]};
            unsigned char destIPBytes[] = {data[30], data[31], data[32], data[33]};
            string packetSource = parseIP(sourceIPBytes);
            string packetDest = parseIP(destIPBytes);

            //check if packet is UDP (17)
            if (data[24] == 17) {
                if (sourceIp == parseIP(sourceIPBytes) && destIp == parseIP(destIPBytes)) {
                    requestTimestamps[getSeqNum(data)] = header->ts;
                } else if (sourceIp == packetDest && destIp == packetSource) {
                    responseTimestamps[getSeqNum(data)] = header->ts;
                }
                //TODO: Richtige SeqNumBytes herrausfinden (40 und 42 sind für ICMP)
                unsigned char seqNumBytes[] = {data[40], data[41]};
                seqNums.push_back(parseNumberFromBytes(seqNumBytes, 2));
            }
        }
        pcap_close(pcap);

        vector<struct timeval> delays = getDelays(requestTimestamps, responseTimestamps);
        vector<bool> lossTrace = getLoss(seqNums);
    }

}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}