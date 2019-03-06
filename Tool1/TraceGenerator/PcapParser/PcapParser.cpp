//
// Created by drieke on 25.01.19.
//

#include "PcapParser.h"

unsigned int PcapParser::parseNumberFromBytes(const unsigned char *bytes, int length) {
    unsigned int number = 0;
    for (int i = length - 1; i >= 0; i--) {
        number += bytes[length - i - 1] * pow(256, i);
    }
    return number;
}

string PcapParser::parseIP(unsigned char *bytes) {
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

unsigned long PcapParser::getSeqNumMobileTool(const u_char *data) {
    //44 = payload start
    const int longDigits = 10;
    unsigned int seqNumDigits[longDigits] = {0};
    for (int i = 0; i < longDigits; i++) {
        seqNumDigits[i] = data[1041 - i] - '0';
    }

    unsigned long seqNum = 0;
    for (int i = 0; i < longDigits; i++) {
        unsigned long factor = 1;
        for (int j = 0; j < i; j++) {
            factor *= 10;
        }
        seqNum += seqNumDigits[i] * factor;
    }
    return seqNum;
}


uint64_t PcapParser::getMillisFromTimeval(struct timeval tv) {
    return (tv.tv_sec * (uint64_t) 1000) + (tv.tv_usec / 1000);
}

/**
 * Calculates the Delay, Loss, Reordering and Duplication
 * @param values the values from the pcaps
 * @return the resutls
 */
struct result *PcapParser::getResults(struct pcapValues *values) {
    auto *results = new struct result;
    auto *delays = new vector<int64_t>();
    auto *loss = new vector<bool>();

    /*
     * Delays and Loss
     */
    int lossCounter = 0;
    for (auto &send: *values->send) {
        uint64_t delay{};
        auto recieved = values->received->find(send.first);

        uint64_t recievedTs = getMillisFromTimeval(recieved->second);
        uint64_t sendTs = getMillisFromTimeval(send.second);

        if (recieved != values->received->end()) {
            //Paket kommt an
            if (lossCounter > 0) {
                //Hole losses nach
                for (int i = 1; i < lossCounter - 1; i++) {
                    loss->push_back(false);
                    delay = 0;
                }
                lossCounter = 0;
            }

            delay = recievedTs - sendTs;
            loss->push_back(true);

            delays->push_back(delay);
        } else {
            //loss
            lossCounter++;
        }
    }
    cout << "delays and loss finished" << endl;

    /*
     * Reordering
     */
    set<unsigned long> reorders;
    unsigned long lastValidSeqNum = 0UL - 1UL;
    for (unsigned long i = (values->seqNumsReceived->size() - 1); i > 0; i--) {
        unsigned long currentSeqNum = values->seqNumsReceived->at(i);
        if (currentSeqNum < lastValidSeqNum) {
            lastValidSeqNum = currentSeqNum;
        } else if (currentSeqNum > lastValidSeqNum) {
            reorders.insert(currentSeqNum);
        }
    }

    double reordering = (double) 100 / (double) values->seqNumsReceived->size() * (double) reorders.size();
    cout << reorders.size() << " reordered packets [" << reordering << "%]" << endl;

    /*
     * Duplication
     */
    set<unsigned long> allSeqNums;

    for (unsigned long recvSeqNum : *values->seqNumsReceived) {
        allSeqNums.insert(recvSeqNum);
    }

    unsigned long duplications = values->seqNumsReceived->size() - allSeqNums.size();
    double duplication = (double) 100 / (double) values->seqNumsReceived->size() * (double) duplications;

    cout << duplications << " duplicated packets [" << duplication << "%]" << endl;


    results->delays = delays;
    results->duplication = duplications;
    results->loss = loss;
    results->reordering = reordering;
    return results;
}

struct pcapValues *
PcapParser::readMobilePcapFile(const string &filename, const string &ipOfPcapDevice, const string &ipOfOtherDevice) {
    auto *toOtherTS = new map<unsigned long, struct timeval>();
    auto *toSelfTS = new map<unsigned long, struct timeval>();
    auto *timestampsSend = new vector<unsigned long>();
    auto *timestampsReceived = new vector<unsigned long>();

    auto *result = new struct pcapValues();

    struct stat buffer{};
    if (stat(filename.c_str(), &buffer) != 0) {
        cout << "file " << filename << " not found" << endl;
    } else {
        char errbuff[PCAP_ERRBUF_SIZE];
        pcap_t *pcap = pcap_open_offline(filename.c_str(), errbuff);
        struct pcap_pkthdr *header;
        const u_char *data;
        bool isFirstPackageToOther = true;
        unsigned long packageCount = 0;
        while (pcap_next_ex(pcap, &header, &data) >= 0) {
            packageCount++;

            unsigned char sourceIPBytes[] = {data[26], data[27], data[28], data[29]};
            unsigned char destIPBytes[] = {data[30], data[31], data[32], data[33]};
            string packetSource = parseIP(sourceIPBytes);
            string packetDest = parseIP(destIPBytes);

            //check if packet is UDP (17)
            unsigned char lengthBytes[] = {data[16], data[17]};
            unsigned long length = parseNumberFromBytes(lengthBytes, 2);
            if (length == 1028) {
                unsigned long seqNum = getSeqNumMobileTool(data);
                if (ipOfPcapDevice == packetSource && ipOfOtherDevice == packetDest) {
                    if (isFirstPackageToOther) {
                        isFirstPackageToOther = false;
                    } else {
                        toOtherTS->insert(pair<unsigned long, struct timeval>(seqNum, header->ts));
                        timestampsSend->push_back(seqNum);
                    }
                } else if (ipOfPcapDevice == packetDest && ipOfOtherDevice == packetSource) {
                    toSelfTS->insert(pair<unsigned long, struct timeval>(seqNum, header->ts));
                    timestampsReceived->push_back(seqNum);
                }
            }
        }
        pcap_close(pcap);

        result->send = toOtherTS;
        result->received = toSelfTS;
        result->seqNumsReceived = timestampsReceived;
        result->seqNumsSend = timestampsSend;

        return result;
    }
    return result;
}


struct result *
PcapParser::startParsing(const string &clientTraceFile, const string &serverTraceFile, const string &globalClientIp,
                         const string &localClientIp, const string &serverIp) {
    struct pcapValues *clientValues = readMobilePcapFile(clientTraceFile, localClientIp, serverIp);
    struct pcapValues *serverValues = readMobilePcapFile(serverTraceFile, serverIp, globalClientIp);

    auto *downloadValues = new struct pcapValues();

    downloadValues->seqNumsSend = serverValues->seqNumsSend;
    downloadValues->seqNumsReceived = clientValues->seqNumsReceived;
    downloadValues->send = serverValues->send;
    downloadValues->received = clientValues->received;
    delete (clientValues);
    delete (serverValues);
    struct result *res = getResults(downloadValues);
    delete (downloadValues->received);
    delete (downloadValues->send);
    delete (downloadValues->seqNumsReceived);
    delete (downloadValues->seqNumsSend);
    delete (downloadValues);
    return res;
}