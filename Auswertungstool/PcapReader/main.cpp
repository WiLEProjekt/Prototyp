#include <iostream>
#include <pcap.h>
#include <sys/stat.h>
#include <cmath>
#include <utility>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
#include <string>

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
    vector<int64_t> delays;
};

struct pcapValues {
    map<unsigned long, struct timeval> send;
    map<unsigned long, struct timeval> received;
    vector<unsigned long> seqNumsSend;
    vector<unsigned long> seqNumsReceived;
};

struct pingValues {
    map<unsigned long, struct timeval> send;
    map<unsigned long, struct timeval> received;
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

unsigned long getSeqNumPing(const u_char *data){
    unsigned char seqNumBytes[] = {data[40], data[41]};
    unsigned long seqNum = parseNumberFromBytes(seqNumBytes, 2);
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

uint64_t getMillisFromTimeval(struct timeval tv){
    return  (tv.tv_sec * (uint64_t)1000) + (tv.tv_usec / 1000);
}

uint64_t getMicrosFromTimeval(struct timeval tv){
    return (tv.tv_sec *(uint64_t)1000000) + tv.tv_usec;
}

vector<double> getPingResults(struct pingValues clientValues, struct pingValues serverValues){
    vector<double> delays;
    for(auto& clientSend: clientValues.send) {
        uint64_t delay{};
        auto serverReceived = serverValues.received.find(clientSend.first);
        if (serverReceived != serverValues.received.end()) {
            uint64_t clientSendMicros = getMicrosFromTimeval(clientSend.second);
            uint64_t serverReceivedMicros = getMicrosFromTimeval(serverReceived->second);

            auto serverSend = serverValues.send.find(clientSend.first);
            if(serverSend != serverValues.send.end()) {
                auto clientReceived = clientValues.received.find(serverSend->first);
                if(clientReceived != clientValues.received.end()) {
                    uint64_t serverSendMicros = getMicrosFromTimeval(serverSend->second);
                    uint64_t clientReceivedMicros = getMicrosFromTimeval(clientReceived->second);

                    int64_t clientToServerDelay = serverReceivedMicros - clientSendMicros;
                    int64_t serverToClientDelay = clientReceivedMicros - serverSendMicros;
                    double result = clientToServerDelay + serverToClientDelay;
                    result /= 1000;
                    delays.push_back(result);
                }
            }
        }
    }
    return delays;
}

struct result getResults(struct pcapValues values){
    struct result results {};
    vector<int64_t> delays;
    vector<bool> loss;
    vector<unsigned long> skippedPackages;
    vector<long> duplications;

    /*
     * Delays and Loss
     */
    for(auto& send: values.send){
        uint64_t delay {};
        auto recieved = values.received.find(send.first);
        if(recieved != values.received.end()) {
            uint64_t recievedTs = getMillisFromTimeval(recieved->second);
            uint64_t sendTs = getMillisFromTimeval(send.second);
            delay = recievedTs - sendTs;
            loss.push_back(true);
        } else {
            delay = 0;
            loss.push_back(false);
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
    map<unsigned long, unsigned long> duplictationsFound;
    for(unsigned long sendSeqNum : values.seqNumsSend){
        duplictationsFound[sendSeqNum] = 0;
    }
    for(unsigned long recSeqNum : values.seqNumsReceived){
        duplictationsFound[recSeqNum] += 1;
    }
    /*for (unsigned long sendSeqNum : values.seqNumsSend) {
        vector<unsigned long> matches;
        copy_if(values.seqNumsReceived.begin(), values.seqNumsReceived.end(), back_inserter(matches), [&](unsigned long v){
            return v == sendSeqNum;
        });
        duplications.push_back(matches.size() - 1);
    }*/
    for(auto& dupli : duplictationsFound){
        duplications.push_back(dupli.second);
    }

    cout << "duplication finished" << endl;

    results.delays = delays;
    results.loss = loss;
    results.packetsSkipped = skippedPackages;
    results.duplications = duplications;
    return results;
}

struct pingValues readPingFile(const string& filename, const string &ipOfPcapDevice, const string& ipOfOtherDevice) {
    map<unsigned long, struct timeval> toOtherTS;
    map<unsigned long, struct timeval> toSelfTS;
    struct pingValues result{};

    struct stat buffer{};
    if (stat(filename.c_str(), &buffer) != 0) {
        cout << "file " << filename << " not found" << endl;
    } else {
        char errbuff[PCAP_ERRBUF_SIZE];
        pcap_t *pcap = pcap_open_offline(filename.c_str(), errbuff);
        struct pcap_pkthdr *header;
        const u_char *data;

        while (pcap_next_ex(pcap, &header, &data) >= 0) {
            //Data[23] == 1: ICMP
            if (data[23] == 1) {
                unsigned char sourceIPBytes[] = {data[26], data[27], data[28], data[29]};
                unsigned char destIPBytes[] = {data[30], data[31], data[32], data[33]};
                string packetSource = parseIP(sourceIPBytes);
                string packetDest = parseIP(destIPBytes);

                unsigned long seqNum = getSeqNumPing(data);

                if (ipOfPcapDevice == packetSource && ipOfOtherDevice == packetDest) {
                    toOtherTS[seqNum] = header->ts;
                } else if (ipOfPcapDevice == packetDest && ipOfOtherDevice == packetSource) {
                    toSelfTS[seqNum] = header->ts;
                }
            }
        }
        pcap_close(pcap);
        result.send = toOtherTS;
        result.received = toSelfTS;
    }
    return result;
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
        //BRAUCHEN WIR DAS NOCH??????
        bool isFirstPackageToOther = true;
        bool isFirstPackageToSelf = true;
        //-------
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
        result.received = toSelfTS;
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

void writeDelayFile(const string& filename, vector<int64_t> delays){
    ofstream uploadDelayFile;
    uploadDelayFile.open(filename);
    for(int64_t ts: delays){
        uploadDelayFile << ts << ";";
    }
    uploadDelayFile.flush();
    uploadDelayFile.close();
}

void writeDelayFile(const string& filename, vector<double> delays){
    ofstream uploadDelayFile;
    uploadDelayFile.open(filename);
    for(double ts: delays){
        uploadDelayFile << ts << ";";
    }
    uploadDelayFile.flush();
    uploadDelayFile.close();
}

void writeResultToFile(const string &path, result upload, result download){
    ofstream uploadLossFile;
    string pathcommand = "mkdir -p " + path;
    system(pathcommand.c_str());
    uploadLossFile.open(path + "uploadLoss.txt");
    for(bool b : upload.loss){
        uploadLossFile << b;
    }
    uploadLossFile.flush();
    uploadLossFile.close();

    ofstream downloadLossFile;
    downloadLossFile.open(path + "downloadLoss.txt");
    for(bool b : download.loss){
        downloadLossFile << b;
    }
    downloadLossFile.flush();
    downloadLossFile.close();

    string uploadDelayFilename = path + "uploadDelays.csv";
    writeDelayFile(uploadDelayFilename, upload.delays);

    string downloadDelayFilename = path + "downloadDelays.csv";
    writeDelayFile(downloadDelayFilename, download.delays);

    ofstream uploadDuplicationFile;
    string filename = path + "uploadDuplication.txt";
    uploadDuplicationFile.open(filename);
    for(long dupli : upload.duplications){
        uploadDuplicationFile << dupli << ";";
    }
    uploadDuplicationFile.flush();
    uploadDuplicationFile.close();

    ofstream downloadDuplicationFile;
    downloadDuplicationFile.open(path + "downloadDuplication.txt");
    for(long dupli : download.duplications){
        downloadDuplicationFile << dupli << ";";
    }
    downloadDuplicationFile.flush();
    downloadDuplicationFile.close();

    ofstream uploadReorderingFile;
    uploadReorderingFile.open(path + "uploadReordering.txt");
    for(unsigned long reor : upload.packetsSkipped){
        uploadReorderingFile << reor << ";";
    }
    uploadReorderingFile.flush();
    uploadReorderingFile.close();

    ofstream downloadReorderingFile;
    downloadReorderingFile.open(path + "downloadReordering.txt");
    for(unsigned long reor : download.packetsSkipped){
        downloadReorderingFile << reor << ";";
    }
    downloadReorderingFile.flush();
    downloadReorderingFile.close();
}


void writeResultToFile(result upload, result download) {
    string path = "Ergebnis/";
    writeResultToFile(path, std::move(upload), std::move(download));
}

vector<double> readPingLog(const string &filename) {
    vector<double> pingResults;
    ifstream file;
    file.open(filename, ios::in);
    string line;
    while(getline(file, line)){
        double ping = stod(line);
        pingResults.push_back(ping);
    }
    file.close();
    return pingResults;
}

int main(int argc, char **argv) {
    if (argc < 5) {
        cout << "PcapReader [client.pcap] [server.pcap] [serverIp] [locale clientIp] [global clientIp]" << endl;
        cout << "PcapReader [path to pcaps] [serverIp] [locale clientIp] [global clientIp]" << endl;
        cout << "PcapReader [path to pcaps] [serverIp] [clientIp] -p" << endl;
        return -1;
    }
    string clientFilename;
    string serverFilename;
    string serverIp;
    string localeClientIp;
    string globalClientIp = argv[4];

    if(globalClientIp == "-p"){
        string path = argv[1];
        serverIp = argv[2];
        string clientIp = argv[3];
        clientFilename = path + "/pingClient.pcap";
        serverFilename = path + "/pingServer.pcap";
        string pinglogFilename = path + "/ping.txt";

        struct pingValues clientValues = readPingFile(clientFilename, clientIp, serverIp);
        struct pingValues serverValues = readPingFile(serverFilename, serverIp, clientIp);

        vector<double> delays = getPingResults(clientValues, serverValues);
        vector<double> pings = readPingLog(pinglogFilename);
        vector<double> results;
        if(delays.size() == pings.size()){
            for(int i = 0; i < delays.size(); i++){
                results.push_back(delays[i] - pings[i]);
            }
        } else {
            cerr << "ERROR pcaps results: " << delays.size() << " ping results: " << pings.size() << endl;
        }

        writeDelayFile(path + "/result.csv", results);
    } else {
        if (argc == 6) {
            clientFilename = argv[1];
            serverFilename = argv[2];
            serverIp = argv[3];
            localeClientIp = argv[4];
            globalClientIp = argv[5];

            struct pcapValues clientValues = readPcapFile(clientFilename, localeClientIp, serverIp);
            struct pcapValues serverValues = readPcapFile(serverFilename, serverIp, globalClientIp);

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

            struct result uploadResult = getResults(uploadValues);
            struct result downloadResult = getResults(downloadValues);

            //printResult(uploadLoss, downloadLoss, uploadDelays, downloadDelays);
            writeResultToFile(uploadResult, downloadResult);
        } else if (argc == 5) {
            string path = argv[1];
            serverIp = argv[2];
            localeClientIp = argv[3];
            globalClientIp = argv[4];
            for (int i = 0; i < 2; i++) {
                string cbrMode = "fast";
                if (i == 0) {
                    cbrMode = "slow";
                }
                clientFilename = path + "/client_cbr_" + cbrMode + ".pcap";
                serverFilename = path + "/server_cbr_" + cbrMode + ".pcap";

                struct pcapValues clientValues = readPcapFile(clientFilename, localeClientIp, serverIp);
                struct pcapValues serverValues = readPcapFile(serverFilename, serverIp, globalClientIp);

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

                struct result uploadResult = getResults(uploadValues);
                struct result downloadResult = getResults(downloadValues);

                string resultPath = path + "/Ergebnis_" + cbrMode + "/";
                //printResult(uploadLoss, downloadLoss, uploadDelays, downloadDelays);
                writeResultToFile(resultPath, uploadResult, downloadResult);
            }
        }
    }



    return 0;
}
