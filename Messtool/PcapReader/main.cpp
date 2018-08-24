#include <iostream>
#include <pcap.h>
#include <sys/stat.h>
#include <cmath>
#include <vector>
#include <map>

using namespace std;

const unsigned int MAX_SEQ_NUM = 4294967295;

struct results {
    vector<struct timeval> delays;
    vector<bool> loss;
};

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
        if (i < 3) {
            ip += ".";
        }
    }
    return ip;
}

unsigned int getSeqNum(const u_char *data) {
    //Data start: 42
    unsigned char seqNumBytes[] = {data[42], data[43], data[44], data[45]};
    unsigned int seqNum = parseNumberFromBytes(seqNumBytes, 4);
    cout << "seqNum: " << seqNum << ": " << (int) data[42] << " " << (int) data[43] << " " << (int) data[44] << " "
         << (int) data[45]
         << endl;
    return seqNum;
}

struct timeval findResponse(unsigned int seqNum, const map<unsigned int, struct timeval> &responseTimestamps) {
    auto pos = responseTimestamps.find(seqNum);
    if (pos == responseTimestamps.end()) {
        cout << "Error at finding seqNum " << seqNum << " size is: " << responseTimestamps.size() << endl;
        struct timeval error{};
        error.tv_sec = -1;
        return error;
    } else {
        cout << "found seqNum " << seqNum << " size is: " << responseTimestamps.size() << endl;
        return pos->second;
    }
}

vector<struct timeval>
getDelays(map<unsigned int, struct timeval> requestTimestamps,
          const map<unsigned int, struct timeval> &responseTimestamps) {
    vector<struct timeval> delays;
    for (const auto &iterator : requestTimestamps) {
        struct timeval delay{};
        struct timeval responseTimestamp = findResponse(iterator.first, responseTimestamps);
        if (responseTimestamp.tv_sec != -1) {
            delay.tv_sec = (responseTimestamp.tv_sec - iterator.second.tv_sec) / 2;
            delay.tv_usec = (responseTimestamp.tv_usec - iterator.second.tv_usec) / 2;
            delays.push_back(delay);
        }
    }

    return delays;
}

/**
 * searches for missing sequence numbers and calculates the loss-trace
 * @param sequenceNumbers all sequence numbers
 * @param lastSeqNum the last outgoing sequence number
 * @return the loss-trace
 */
vector<bool> findMissingSeqNums(vector<unsigned int> sequenceNumbers, unsigned int lastSeqNum) {
    vector<bool> calculatedLosses;
    unsigned int seqNumBefore = 0; //important initialization => detects if first few packages are lost
    unsigned long packetCounter = 0;
    bool firstSeqNumRed = false;

    for (unsigned int seqNum : sequenceNumbers) {
        if (firstSeqNumRed) {
            int diff = seqNum - seqNumBefore;
            if (diff > 1) { //packets are lost in ascending order, maximum sequencenumber is not yet reached
                for (int i = 0; i < diff - 1; i++) { //Push lost packets
                    packetCounter++;
                    calculatedLosses.push_back(false);
                }
                calculatedLosses.push_back(true); //Push found packet
                packetCounter++;
            } else if (diff <= 0) { //Packet loss at maximum sequencenumber detected
                diff = MAX_SEQ_NUM - seqNumBefore + seqNum;
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
        seqNumBefore = seqNum;
    }

    for (unsigned int i = 0; i < lastSeqNum - seqNumBefore; i++) {
        calculatedLosses.push_back(false);
    }

    return calculatedLosses;
}

vector<bool> getLoss(vector<unsigned int> seqNums, unsigned int lastSeqNum) {
    vector<bool> trace;
    trace = findMissingSeqNums(move(seqNums), lastSeqNum);
    return trace;
}

void printPacket(struct pcap_pkthdr *header, const u_char *data) {
    for (u_int i = 0; (i < header->caplen); i++) {
        // Start printing on the next after every 16 octets
        if ((i % 16) == 0) printf("\n");

        // Print each octet as hex (x), make sure there is always two characters (.2).
        printf("%.2x ", data[i]);
    }
}

struct results
readPcapFile(const string &clientPcapFile, const string &serverPcapFile, const string &sourceIp, const string &destIp) {
    map<unsigned int, struct timeval> requestTimestamps;
    map<unsigned int, struct timeval> responseTimestamps;
    vector<unsigned int> seqNums;
    unsigned int lastSeqNum = 0;
    struct bpf_program fp{};
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 mask;
    bpf_u_int32 net;

    char *dev = pcap_lookupdev(errbuf);
    struct stat buffer{};
    if (stat(clientPcapFile.c_str(), &buffer) != 0) {
        cout << "file " << clientPcapFile << " not found" << endl;
    } else {
        pcap_lookupnet(dev, &net, &mask, errbuf);
        pcap_t *pcap = pcap_open_offline(clientPcapFile.c_str(), errbuf);
        string filter = "udp and dst host " + destIp + " and src host " + sourceIp;
        pcap_compile(pcap, &fp, filter.c_str(), 0, net);
        pcap_setfilter(pcap, &fp);
        struct pcap_pkthdr *header;
        const u_char *data;

        while (pcap_next_ex(pcap, &header, &data) >= 0) {
            if (header->len != header->caplen) {
                printf("Warning! Capture size different than packet size: %1d bytes\n", header->caplen);
            }

            //printPacket(header, data);

            unsigned int seqNumber = getSeqNum(data);
            lastSeqNum = seqNumber;
            requestTimestamps[seqNumber] = header->ts;
            seqNums.push_back(seqNumber);
        }

        pcap = pcap_open_offline(serverPcapFile.c_str(), errbuf);
        filter = "udp and src host " + sourceIp + " and dst host " + destIp;
        pcap_compile(pcap, &fp, filter.c_str(), 0, net);
        pcap_setfilter(pcap, &fp);

        while (pcap_next_ex(pcap, &header, &data) >= 0) {
            if (header->len != header->caplen) {
                printf("Warning! Capture size different than packet size: %1d bytes\n", header->caplen);
            }

            //printPacket(header, data);

            unsigned int seqNumber = getSeqNum(data);
            lastSeqNum = seqNumber;
            responseTimestamps[seqNumber] = header->ts;
            seqNums.push_back(seqNumber);
        }

        pcap_close(pcap);

        vector<struct timeval> delays = getDelays(requestTimestamps, responseTimestamps);
        vector<bool> lossTrace = getLoss(seqNums, lastSeqNum);
        struct results result{};
        result.delays = delays;
        result.loss = lossTrace;

        return result;
    }

    struct results result{};

    return result;
}

int main(int argc, char **argv) {
    if (argc < 4) {
        cout << "PcapReader [client.pcap] [server.pcap] [sourceIp] [destIp]";
        return -1;
    }
    string clientPcapFile = argv[1];
    string serverPcapFile = argv[2];
    string sourceIp = argv[3];
    string destIp = argv[4];
    struct results result = readPcapFile(clientPcapFile, serverPcapFile, sourceIp, destIp);

    cout << "Delay:" << endl;
    for (struct timeval delay : result.delays) {
        cout << delay.tv_sec << "s " << delay.tv_usec << "ms" << endl;
    }

    cout << "Loss:" << endl;
    for (bool loss: result.loss) {
        cout << loss;
    }

    return 0;
}