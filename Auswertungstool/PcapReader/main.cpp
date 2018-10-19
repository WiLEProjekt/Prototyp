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
    vector<long> duplications;
    vector<struct timeval> delays;
};

struct pcapValues {
    map<unsigned long, struct timeval> send;
    map<unsigned long, struct timeval> recieved;
    vector<unsigned long> seqNumsSend;
    vector<unsigned long> seqNumsReceived;
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

unsigned long getIndexOf(vector<unsigned long> searchVector, unsigned long value){
    for(unsigned long i = 0; i < searchVector.size(); i++){
        if(searchVector[i] == value){
            return i;
        }
    }

    return searchVector.size() + 1;
}

struct result getResults(pcapValues values){
    struct result results {};
    vector<struct timeval> delays;
    vector<bool> loss;
    vector<unsigned long> skippedPackages;
    vector<long> duplications;

    /*
     * Delays and Loss
     */
    for(auto& send: values.send){
        struct timeval delay {};
        auto recieved = values.recieved.find(send.first);
        if(recieved != values.recieved.end()) {
            delay.tv_sec = recieved->second.tv_sec - send.second.tv_sec;
            delay.tv_usec = recieved->second.tv_usec - send.second.tv_usec;
            loss.push_back(false);
        } else {
            delay.tv_sec = -1;
            delay.tv_usec = -1;
            loss.push_back(true);
        }
        delays.push_back(delay);
    }

    cout << "delays and loss finished" << endl;

    /*
     * Reordering
     */
    unsigned long losses = 0;
    for(unsigned long i = 0; i < values.seqNumsSend.size(); i++){
        unsigned long currentSeqNum = values.seqNumsSend[i];
        bool isloss = loss[i+1];
        if(isloss){
            skippedPackages.push_back(0);
            losses++;
        } else {
            bool seqNumFound = false;
            unsigned long j;
            for(j = 0; i + j - losses < values.seqNumsReceived.size() && !seqNumFound; j++){
                unsigned long seqNum = values.seqNumsReceived[i + j - losses];
                if(seqNum == currentSeqNum){
                    skippedPackages.push_back(j);
                    seqNumFound = true;
                }
            }
        }
    }

    cout << skippedPackages.size() << " reordering finished" << endl;
    /*
     * Duplication
     */
    for (unsigned long sendSeqNum : values.seqNumsSend) {
        vector<unsigned long> matches;
        copy_if(values.seqNumsReceived.begin(), values.seqNumsReceived.end(), back_inserter(matches), [&](unsigned long v){
            return v == sendSeqNum;
        });
        duplications.push_back(matches.size() - 1);
    }

    cout << "duplication finished" << endl;

    results.delays = delays;
    results.loss = loss;
    results.packetsSkipped = skippedPackages;
    results.duplications = duplications;
    return results;
}

struct pcapValues readPcapFile(const string &filename, const string &ipOfPcapDevice, const string &ipOfOtherDevice) {
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
                        timestampsSend.push_back(seqNum);
                    }
                } else if (ipOfPcapDevice == packetDest && ipOfOtherDevice == packetSource) {
                    if(isFirstPackageToSelf) {
                        isFirstPackageToSelf = false;
                    } else {
                        toSelfTS[seqNum] = header->ts;
                        timestampsReceived.push_back(seqNum);
                    }
                }
            }
        }
        pcap_close(pcap);

        result.send = toOtherTS;
        result.recieved = toSelfTS;
        result.seqNumsReceived = timestampsReceived;
        result.seqNumsSend = timestampsSend;

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
    uploadLossFile.flush();
    uploadLossFile.close();

    ofstream downloadLossFile;
    downloadLossFile.open("downloadLoss.txt");
    for(bool b : download.loss){
        downloadLossFile << b;
    }
    downloadLossFile.flush();
    downloadLossFile.close();

    ofstream uploadDelayFile;
    uploadDelayFile.open("uploadDelays.csv");
    for(struct timeval tv : upload.delays){
        uploadDelayFile << tv.tv_sec << "." << tv.tv_usec << ";";
    }
    uploadDelayFile.flush();
    uploadDelayFile.close();

    ofstream downloadDelayFile;
    downloadDelayFile.open("downloadDelays.csv");
    for(struct timeval tv : download.delays){
        downloadDelayFile << tv.tv_sec << "." << tv.tv_usec << ";";
    }
    downloadDelayFile.flush();
    downloadDelayFile.close();

    ofstream uploadDuplicationFile;
    uploadDuplicationFile.open("uploadDuplication.txt");
    for(long dupli : upload.duplications){
        uploadDuplicationFile << dupli << ";";
    }
    uploadDuplicationFile.flush();
    uploadDuplicationFile.close();

    ofstream downloadDuplicationFile;
    downloadDuplicationFile.open("downloadDuplication.txt");
    for(long dupli : download.duplications){
        downloadDuplicationFile << dupli << ";";
    }
    downloadDuplicationFile.flush();
    downloadDuplicationFile.close();

    ofstream uploadReorderingFile;
    uploadReorderingFile.open("uploadReordering.txt");
    for(unsigned long reor : upload.packetsSkipped){
        uploadReorderingFile << reor << ";";
    }
    uploadReorderingFile.flush();
    uploadReorderingFile.close();

    ofstream downloadReorderingFile;
    downloadReorderingFile.open("downloadReordering.txt");
    for(unsigned long reor : download.packetsSkipped){
        downloadReorderingFile << reor << ";";
    }
    downloadReorderingFile.flush();
    downloadReorderingFile.close();
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
    struct pcapValues clientValues = readPcapFile(clientFilename, localeClientIp, serverIp);
    struct pcapValues serverValues = readPcapFile(serverFilename, serverIp, globalClientIp);

    struct pcapValues downloadValues{};
    struct pcapValues uploadValues{};

    downloadValues.seqNumsSend = serverValues.seqNumsSend;
    downloadValues.seqNumsReceived = clientValues.seqNumsReceived;
    downloadValues.send = serverValues.send;
    downloadValues.recieved = clientValues.recieved;

    uploadValues.seqNumsSend = clientValues.seqNumsSend;
    uploadValues.seqNumsReceived = serverValues.seqNumsReceived;
    uploadValues.send = clientValues.send;
    uploadValues.recieved = serverValues.recieved;

    struct result uploadResult = getResults(uploadValues);
    struct result downloadResult = getResults(downloadValues);

    //printResult(uploadLoss, downloadLoss, uploadDelays, downloadDelays);
    writeResultToFile(uploadResult, downloadResult);

    return 0;
}
