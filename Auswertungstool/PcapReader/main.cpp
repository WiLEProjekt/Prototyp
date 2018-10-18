#include <iostream>
#include <pcap.h>
#include <sys/stat.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>

using namespace std;

struct result {
    /**
     * 0 = packet arrived, 1 = packet lost
     */
    vector<bool> loss;
    /**
     * reordering as the amount of skipped packets
     */
    vector<unsigned long> packetsSkipped;
    vector<int> duplications;
    vector<struct timeval> delays;
};

struct pcapValues {
    map<unsigned long, struct timeval> send;
    map<unsigned long, struct timeval> recieved;
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

unsigned long getSeqNum(const u_char *data) {
    //Data start: 42
    unsigned char seqNumBytes[] = {data[50], data[51], data[52], data[53]};
    unsigned long seqNum = parseNumberFromBytes(seqNumBytes, 4);
    return seqNum;
}

vector<bool> getLoss(vector<unsigned long> sendSeqNums, vector<unsigned long> recievedSeqNums) {
    vector<bool> trace;
    for(unsigned long send : sendSeqNums){
        if(std::find(recievedSeqNums.begin(), recievedSeqNums.end(), send) != recievedSeqNums.end()){
            trace.push_back(true);
        } else {
            trace.push_back(false);
        }
    }
    return trace;
}

void printData(const u_char *data, struct pcap_pkthdr *header){
    cout << endl;
    for(u_int i = 0; (i < header->caplen); i++){
        if((i%16) == 0){
            cout << endl;
        }
        printf("%.2x ", data[i]);
    }
}

unsigned long* getTimestamps(const map<unsigned long, struct timeval> &timestampMap){
    unsigned long timestamps[timestampMap.size()];
    unsigned long i = 0;
    for(auto& entity : timestampMap){
        timestamps[i++] = entity.first;
    }
    return timestamps;
}

struct result getDelays(map<unsigned long, struct timeval> sendTS, map<unsigned long, struct timeval> recievedTS){
    struct result results {};
    vector<struct timeval> delays;
    vector<bool> loss;
    vector<unsigned long> skippedPackages;
    vector<int> duplications;
    for(auto& send: sendTS){
        struct timeval delay {};
        auto recieved = recievedTS.find(send.first);
        if(recieved != recievedTS.end()) {
            delay.tv_sec = recieved->second.tv_sec - send.second.tv_sec;
            delay.tv_usec = recieved->second.tv_usec - send.second.tv_usec;
            loss.push_back(true);
        } else {
            delay.tv_sec = -1;
            delay.tv_usec = -1;
            loss.push_back(false);
        }
        delays.push_back(delay);
    }
    unsigned long* sendtimestamps = getTimestamps(sendTS);
    unsigned long* receivedTimestamps = getTimestamps(recievedTS);
    for(unsigned long i = 0; i < sendTS.size(); i++){
        unsigned long currentTS = sendtimestamps[i];
        if(loss[i]){
            skippedPackages.push_back(0);
        } else {
            for(unsigned long j = i; j < recievedTS.size(); j++){
                if(receivedTimestamps[j] == currentTS){
                    
                }
            }
        }
    }

    results.delays = delays;
    results.loss = loss;
    return results;
}

struct pcapValues readPcapFile(const string &filename, const string &ipOfPcapDevice, const string &ipOfOtherDevice) {
    map<unsigned long, struct timeval> toOtherTS;
    map<unsigned long, struct timeval> toSelfTS;

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
        bool isFirstPackageToSelf = true;
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
            if (length == 1478) {
                unsigned long seqNum = getSeqNum(data);
                //printData(data, header);
                if (ipOfPcapDevice == packetSource && ipOfOtherDevice == packetDest) {
                    if(isFirstPackageToOther) {
                        isFirstPackageToOther = false;
                    } else {
                        toOtherTS[seqNum] = header->ts;
                    }
                } else if (ipOfPcapDevice == packetDest && ipOfOtherDevice == packetSource) {
                    if(isFirstPackageToSelf) {
                        isFirstPackageToSelf = false;
                    } else {
                        toSelfTS[seqNum] = header->ts;
                    }
                }
            }
        }
        pcap_close(pcap);

        result.send = toOtherTS;
        result.recieved = toSelfTS;

        return result;
    }

    return result;
}

void printResult(const vector<bool> &clientToServerLoss, const vector<bool> &serverToClientLoss,
                 const vector<timeval> &clientToServerDelays, const vector<timeval> &serverToClientDelays) {
    cout << "Loss Client to Server" << endl;
    for(bool b : clientToServerLoss){
        cout << b << " ";
    }
    cout << endl << "Loss Server to Client" << endl;
    for(bool b : serverToClientLoss) {
        cout << b << " ";
    }
    cout << endl << "Delays Client to Server:" << endl;
    for(struct timeval tv : clientToServerDelays){
        cout << tv.tv_sec << "." << tv.tv_usec << " ";
    }
    cout << endl << "Delays Server to Client:" << endl;
    for(struct timeval tv : serverToClientDelays){
        cout << tv.tv_sec << "." << tv.tv_usec << " ";
    }
}

void writeResultToFile(result upload, result download) {
    ofstream uploadLossFile;
    uploadLossFile.open("uploadLoss.txt");
    for(bool b : upload.loss){
        uploadLossFile << b;
    }
    uploadLossFile.close();

    ofstream downloadLossFile;
    uploadLossFile.open("downloadLoss.txt");
    for(bool b : download.loss){
        downloadLossFile << b;
    }
    downloadLossFile.close();

    ofstream uploadDelayFile;
    uploadDelayFile.open("uploadDelays.csv");
    for(struct timeval tv : upload.delays){
        uploadDelayFile << tv.tv_sec << "." << tv.tv_usec << ";";
    }
    uploadDelayFile.close();

    ofstream downloadDelayFile;
    downloadDelayFile.open("downloadDelays.csv");
    for(struct timeval tv : download.delays){
        downloadDelayFile << tv.tv_sec << "." << tv.tv_usec << ";";
    }
    downloadDelayFile.close();
}

int main(int argc, char **argv) {
    if (argc < 4) {
        cout << "PcapReader [client.pcap] [server.pcap] [serverIp] [locale clientIp] [global clientIp]";
        return -1;
    }
    string clientFilename = argv[1];
    string serverFilename = argv[2];
    string serverIp = argv[3];
    string localeClientIp = argv[4];
    string globalClientIp = argv[5];
    struct pcapValues clientResult = readPcapFile(clientFilename, localeClientIp, serverIp);
    struct pcapValues serverResult = readPcapFile(serverFilename, serverIp, globalClientIp);

    struct result uploadResult = getDelays(clientResult.send, serverResult.recieved);
    struct result downloadResult = getDelays(serverResult.send, clientResult.recieved);

    //printResult(uploadLoss, downloadLoss, uploadDelays, downloadDelays);
    writeResultToFile(uploadResult, downloadResult);

    return 0;
}
