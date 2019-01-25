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

unsigned long PcapParser::getSeqNumIperf(const u_char *data) {
    //Data start: 42
    unsigned char seqNumBytes[] = {data[50], data[51], data[52], data[53]};
    unsigned long seqNum = parseNumberFromBytes(seqNumBytes, 4);
    return seqNum;
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
struct result PcapParser::getResults(struct pcapValues values) {
    struct result results{};
    vector<int64_t> delays;
    vector<bool> loss;
    vector<struct resultPoint> points;

    /*
     * Delays and Loss
     */
    int lossCounter = 0;
    int64_t lastTs = 0;
    for (auto &send: values.send) {
        uint64_t delay{};
        auto recieved = values.received.find(send.first);

        uint64_t recievedTs = getMillisFromTimeval(recieved->second);
        uint64_t sendTs = getMillisFromTimeval(send.second);

        if (recieved != values.received.end()) {
            //Paket kommt an
            if (lossCounter > 0) {
                //Hole losses nach
                int64_t tsDiff = recievedTs - lastTs;
                double step = (double) tsDiff / lossCounter;
                for (int i = 1; i < lossCounter - 1; i++) {
                    auto currTs = (uint64_t) ((double) lastTs + step * i);
                    struct resultPoint lossPoint{};
                    lossPoint.recievedTs = currTs;
                    lossPoint.delay = 0;
                    lossPoint.packetRecieved = false;
                    lossPoint.seqNum = 0;
                    points.push_back(lossPoint);
                    loss.push_back(false);
                    delay = 0;
                }
                lossCounter = 0;
            }

            lastTs = recievedTs;

            delay = recievedTs - sendTs;
            loss.push_back(true);

            struct resultPoint currentPoint{};
            currentPoint.delay = delay;
            currentPoint.recievedTs = recievedTs;
            currentPoint.packetRecieved = true;
            currentPoint.seqNum = recieved->first;
            points.push_back(currentPoint);
            delays.push_back(delay);
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
    for (unsigned long i = (values.seqNumsReceived.size() - 1); i > 0; i--) {
        unsigned long currentSeqNum = values.seqNumsReceived[i];
        if (currentSeqNum < lastValidSeqNum) {
            lastValidSeqNum = currentSeqNum;
        } else if (currentSeqNum > lastValidSeqNum) {
            reorders.insert(currentSeqNum);
        }
    }

    double reordering = (double) 100 / (double) values.seqNumsReceived.size() * (double) reorders.size();
    cout << reorders.size() << " reordered packets [" << reordering << "%]" << endl;

    /*
     * Duplication
     */
    set<unsigned long> allSeqNums;

    for (unsigned long recvSeqNum : values.seqNumsReceived) {
        allSeqNums.insert(recvSeqNum);
    }

    unsigned long duplications = values.seqNumsReceived.size() - allSeqNums.size();
    double duplication = (double) 100 / (double) values.seqNumsReceived.size() * (double) duplications;

    cout << duplications << " duplicated packets [" << duplication << "%]" << endl;


    results.delays = delays;
    results.duplication = duplications;
    results.loss = loss;
    results.reordering = reordering;
    results.fullResult = points;
    return results;
}

struct pcapValues
PcapParser::readMobilePcapFile(const string &filename, const string &ipOfPcapDevice, const string &ipOfOtherDevice) {
    map<unsigned long, struct timeval> toOtherTS;
    map<unsigned long, struct timeval> toSelfTS;
    vector<unsigned long> timestampsSend;
    vector<unsigned long> timestampsReceived;

    struct pcapValues result{};

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
                        toOtherTS[seqNum] = header->ts;
                        timestampsSend.push_back(seqNum);
                    }
                } else if (ipOfPcapDevice == packetDest && ipOfOtherDevice == packetSource) {
                    toSelfTS[seqNum] = header->ts;
                    timestampsReceived.push_back(seqNum);
                }
            }
        }
        pcap_close(pcap);

        result.send = toOtherTS;
        result.received = toSelfTS;
        result.seqNumsReceived = timestampsReceived;
        result.seqNumsSend = timestampsSend;

        return result;
    }

    return result;
}


struct pcapValues
PcapParser::readPcapFile(const string &filename, const string &ipOfPcapDevice, const string &ipOfOtherDevice) {
    map<unsigned long, struct timeval> toOtherTS;
    map<unsigned long, struct timeval> toSelfTS;
    vector<unsigned long> timestampsSend;
    vector<unsigned long> timestampsReceived;

    struct pcapValues result{};

    struct stat buffer{};
    if (stat(filename.c_str(), &buffer) != 0) {
        cout << "file " << filename << " not found" << endl;
    } else {
        char errbuff[PCAP_ERRBUF_SIZE];
        pcap_t *pcap = pcap_open_offline(filename.c_str(), errbuff);
        struct pcap_pkthdr *header;
        const u_char *data;
        unsigned long packageCount = 0;
        while (pcap_next_ex(pcap, &header, &data) >= 0) {
            if (header->len != header->caplen) {
                //printf("Warning! Capture size different than packet size: %1d bytes\n", header->caplen);
            }
            packageCount++;

            unsigned char sourceIPBytes[] = {data[26], data[27], data[28], data[29]};
            unsigned char destIPBytes[] = {data[30], data[31], data[32], data[33]};
            string packetSource = parseIP(sourceIPBytes);
            string packetDest = parseIP(destIPBytes);

            //check if packet is UDP (17)
            unsigned char lengthBytes[] = {data[16], data[17]};
            unsigned long length = parseNumberFromBytes(lengthBytes, 2);
            if (length == 1028) {
                unsigned long seqNum = getSeqNumIperf(data);
                if (ipOfPcapDevice == packetSource && ipOfOtherDevice == packetDest) {
                    toOtherTS[seqNum] = header->ts;
                    timestampsSend.push_back(seqNum);
                } else if (ipOfPcapDevice == packetDest && ipOfOtherDevice == packetSource) {
                    toSelfTS[seqNum] = header->ts;
                    timestampsReceived.push_back(seqNum);
                }
            }
        }
        pcap_close(pcap);

        result.send = toOtherTS;
        result.received = toSelfTS;
        result.seqNumsReceived = timestampsReceived;
        result.seqNumsSend = timestampsSend;

        return result;
    }

    return result;
}

void PcapParser::writeDelayFile(const string &filename, vector<int64_t> delays) {
    ofstream uploadDelayFile;
    uploadDelayFile.open(filename);
    for (int64_t ts: delays) {
        uploadDelayFile << ts << " ";
    }
    uploadDelayFile.flush();
    uploadDelayFile.close();
}

void PcapParser::writeFullTraceFile(const string &filename, vector<resultPoint> result) {
    ofstream uploadDelayFile;
    uploadDelayFile.open(filename);

    for (struct resultPoint rp : result) {
        uploadDelayFile << rp.recievedTs << ";" << rp.delay << ";" << rp.packetRecieved << ";" << rp.seqNum << ";"
                        << rp.sigStrength << ";" << rp.type << endl;
    }
    uploadDelayFile.flush();
    uploadDelayFile.close();
}

void PcapParser::writeResultToFile(const result &download) {
    string path = "Ergebnis/";
    ofstream uploadLossFile;
    string pathcommand = "mkdir -p " + path;
    system(pathcommand.c_str());

    ofstream downloadLossFile;
    downloadLossFile.open(path + "downloadLoss.txt");
    for (bool b : download.loss) {
        downloadLossFile << b;
    }
    downloadLossFile.flush();
    downloadLossFile.close();

    string downloadDelayFilename = path + "downloadDelays.csv";
    writeDelayFile(downloadDelayFilename, download.delays);

    ofstream downloadDuplicationFile;
    downloadDuplicationFile.open(path + "downloadDuplication.txt");
    downloadDuplicationFile << download.duplication << endl;
    downloadDuplicationFile.flush();
    downloadDuplicationFile.close();

    ofstream downloadReorderingFile;
    downloadReorderingFile.open(path + "downloadReordering.txt");
    downloadReorderingFile << download.reordering << endl;
    downloadReorderingFile.flush();
    downloadReorderingFile.close();

    writeFullTraceFile(path + "downloadfull.csv", download.fullResult);
}

struct result PcapParser::startParsing(const string &clientTraceFile, const string &serverTraceFile, const string &globalClientIp,
                         const string &localClientIp, const string &serverIp, bool parameterized){
    struct pcapValues clientValues = readMobilePcapFile(clientTraceFile, localClientIp, serverIp);
    struct pcapValues serverValues = readMobilePcapFile(serverTraceFile, serverIp, globalClientIp);

    struct pcapValues downloadValues{};
    struct pcapValues uploadValues{};

    downloadValues.seqNumsSend = serverValues.seqNumsSend;
    downloadValues.seqNumsReceived = clientValues.seqNumsReceived;
    downloadValues.send = serverValues.send;
    downloadValues.received = clientValues.received;

    uploadValues.seqNumsSend = clientValues.seqNumsSend;
    uploadValues.seqNumsReceived = serverValues.seqNumsReceived;
    uploadValues.send = clientValues.send;
    uploadValues.received = serverValues.received;

    struct result downloadResult = getResults(downloadValues);
}