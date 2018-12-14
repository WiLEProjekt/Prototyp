
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
#include <dirent.h>
#include <experimental/filesystem>
#include <set>

using namespace std;

struct result {
    /**
     * 0 = packet arrived, 1 = packet lost
     */
    vector<bool> loss;
    double reordering;
    double duplication;
    vector<int64_t> delays;

    vector<struct resultpoint> fullResult;
};

struct resultpoint {
    uint64_t ts;
    bool loss;
    unsigned long packtesSkipped;
    long duplications;
    int64_t delay;
};

struct pcapValues {
    map<unsigned long, struct timeval> send;
    map<unsigned long, struct timeval> received;
    vector<unsigned long> seqNumsSend;
    vector<unsigned long> seqNumsReceived;
};

struct pingValues {
    vector<struct timeval> send;
    vector<struct timeval> received;
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

unsigned long getSeqNumIperf(const u_char *data) {
    //Data start: 42
    unsigned char seqNumBytes[] = {data[50], data[51], data[52], data[53]};
    unsigned long seqNum = parseNumberFromBytes(seqNumBytes, 4);
    return seqNum;
}

unsigned long getSeqNumMobileTool(const u_char *data) {
    //44 = payload start
    const int longDigits = 10;
    unsigned int seqNumDigits[longDigits] = {0};
    for(int i = 0; i < longDigits; i++){
        seqNumDigits[i] = data[1041 - i] - '0';
    }

    unsigned long seqNum = 0;
    for(int i = 0; i< longDigits; i++){
        unsigned long factor = 1;
        for(int j = 0; j < i; j++){
            factor *= 10;
        }
        seqNum += seqNumDigits[i] * factor;
    }
    return seqNum;
}

unsigned long getSeqNumPing(const u_char *data) {
    unsigned char seqNumBytes[] = {data[42], data[43]};
    unsigned long seqNum = parseNumberFromBytes(seqNumBytes, 2);
    return seqNum;
}

vector<bool> getLoss(vector<unsigned long> sendSeqNums, vector<unsigned long> recievedSeqNums) {
    vector<bool> trace;
    for (unsigned long send : sendSeqNums) {
        trace.push_back(std::find(recievedSeqNums.begin(), recievedSeqNums.end(), send) != recievedSeqNums.end());
    }
    return trace;
}

void printData(const u_char *data, struct pcap_pkthdr *header) {
    cout << endl;
    for (u_int i = 0; (i < header->caplen); i++) {
        if ((i % 16) == 0) {
            cout << endl;
        }
        printf("%.2x ", data[i]);
    }
}

unsigned long getIndexOf(vector<unsigned long> searchVector, unsigned long value) {
    for (unsigned long i = 0; i < searchVector.size(); i++) {
        if (searchVector[i] == value) {
            return i;
        }
    }

    return searchVector.size() + 1;
}

uint64_t getMillisFromTimeval(struct timeval tv) {
    return (tv.tv_sec * (uint64_t) 1000) + (tv.tv_usec / 1000);
}

uint64_t getMicrosFromTimeval(struct timeval tv) {
    return (tv.tv_sec * (uint64_t) 1000000) + tv.tv_usec;
}

vector<double> getPingResults(vector<struct timeval> clientValues, vector<struct timeval> serverValues) {
    vector<double> delays;

    for (unsigned long i = 0; i < serverValues.size(); i++) {
        if (i % 2 == 1) {
            uint64_t clientSendMicros = getMicrosFromTimeval(clientValues[i - 1]);
            uint64_t serverReceivedMicros = getMicrosFromTimeval(serverValues[i - 1]);
            uint64_t serverSendMicros = getMicrosFromTimeval(serverValues[i]);
            uint64_t clientReceivedMicros = getMicrosFromTimeval(clientValues[i]);

            int64_t clientToServerDelay = serverReceivedMicros - clientSendMicros;
            int64_t serverToClientDelay = clientReceivedMicros - serverSendMicros;
            double result = clientToServerDelay + serverToClientDelay;
            result /= 1000;
            delays.push_back(result);
        }
    }
    return delays;
}

/**
 * Calculates the Delay, Loss, Reordering and Duplication
 * @param values the values from the pcaps
 * @return the resutls
 */
struct result getResults(struct pcapValues values) {
    struct result results{};
    vector<int64_t> delays;
    vector<bool> loss;
    vector<struct resultpoint> points;

    /*
     * Delays and Loss
     */
    for (auto &send: values.send) {
        bool isLoss;
        uint64_t delay{};
        auto recieved = values.received.find(send.first);
        if (recieved != values.received.end()) {
            uint64_t recievedTs = getMillisFromTimeval(recieved->second);
            uint64_t sendTs = getMillisFromTimeval(send.second);
            delay = recievedTs - sendTs;
            loss.push_back(true);
            isLoss = true;
        } else {
            delay = 0;
            loss.push_back(false);
            isLoss = false;
        }
        delays.push_back(delay);
        struct resultpoint currentPoint{};
        currentPoint.loss = isLoss;
        currentPoint.delay = delay;
        currentPoint.ts = recieved;
        points.push_back(currentPoint);
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
    results.loss = loss;
    results.duplication = duplications;
    results.reordering = reordering;
    results.fullResult = points;
    return results;
}

vector<struct timeval>
readPingFile(const string &filename, const string &ipOfPcapDevice, const string &ipOfOtherDevice) {
    vector<struct timeval> timeStamps;
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
            timeStamps.push_back(header->ts);
        }
        pcap_close(pcap);
    }
    return timeStamps;
}

struct pcapValues readMobilePcapFile(const string &filename, const string &ipOfPcapDevice, const string &ipOfOtherDevice) {
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
            if (length == 1028) {
                unsigned long seqNum = getSeqNumIperf(data);
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

void printResult(const vector<bool> &clientToServerLoss, const vector<bool> &serverToClientLoss,
                 const vector<timeval> &clientToServerDelays, const vector<timeval> &serverToClientDelays) {
    cout << "Loss Client to Server" << endl;
    for (bool b : clientToServerLoss) {
        cout << b << " ";
    }
    cout << endl << "Loss Server to Client" << endl;
    for (bool b : serverToClientLoss) {
        cout << b << " ";
    }
    cout << endl << "Delays Client to Server:" << endl;
    for (struct timeval tv : clientToServerDelays) {
        cout << tv.tv_sec << "." << tv.tv_usec << " ";
    }
    cout << endl << "Delays Server to Client:" << endl;
    for (struct timeval tv : serverToClientDelays) {
        cout << tv.tv_sec << "." << tv.tv_usec << " ";
    }
}

void writeDelayFile(const string &filename, vector<int64_t> delays) {
    ofstream uploadDelayFile;
    uploadDelayFile.open(filename);
    for (int64_t ts: delays) {
        uploadDelayFile << ts << endl;
    }
    uploadDelayFile.flush();
    uploadDelayFile.close();
}

void writeDelayFile(const string &filename, vector<double> delays) {
    ofstream uploadDelayFile;
    uploadDelayFile.open(filename);
    for (double ts: delays) {
        uploadDelayFile << ts << endl;
    }
    uploadDelayFile.flush();
    uploadDelayFile.close();
}

void writeFullTraceFile(const string &filename, vector<resultpoint> result) {
    ofstream uploadDelayFile;
    uploadDelayFile.open(filename);
    for (struct resultpoint rp : result) {
        uploadDelayFile << rp.ts << ";" << rp.delay << ";" << rp.loss << endl;
    }
    uploadDelayFile.flush();
    uploadDelayFile.close();
}

void writeResultToFile(const string &path, result upload, result download) {
    ofstream uploadLossFile;
    string pathcommand = "mkdir -p " + path;
    system(pathcommand.c_str());
    uploadLossFile.open(path + "uploadLoss.txt");
    for (bool b : upload.loss) {
        uploadLossFile << b;
    }
    uploadLossFile.flush();
    uploadLossFile.close();

    ofstream downloadLossFile;
    downloadLossFile.open(path + "downloadLoss.txt");
    for (bool b : download.loss) {
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
    uploadDuplicationFile << upload.duplication << endl;
    uploadDuplicationFile.flush();
    uploadDuplicationFile.close();

    ofstream downloadDuplicationFile;
    downloadDuplicationFile.open(path + "downloadDuplication.txt");
    downloadDuplicationFile << download.duplication << endl;
    downloadDuplicationFile.flush();
    downloadDuplicationFile.close();

    ofstream uploadReorderingFile;
    uploadReorderingFile.open(path + "uploadReordering.txt");
    uploadReorderingFile << upload.reordering << endl;
    uploadReorderingFile.flush();
    uploadReorderingFile.close();

    ofstream downloadReorderingFile;
    downloadReorderingFile.open(path + "downloadReordering.txt");
    downloadReorderingFile << download.reordering << endl;
    downloadReorderingFile.flush();
    downloadReorderingFile.close();

    writeFullTraceFile(path + "uploadfull.csv", upload.fullResult);
    writeFullTraceFile(path + "downloadfull.csv", download.fullResult);
}


void writeResultToFile(result upload, result download) {
    string path = "Ergebnis/";
    writeResultToFile(path, std::move(upload), std::move(download));
}

void writeResultToFile(const result &download){
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

vector<double> readPingLog(const string &filename) {
    vector<double> pingResults;
    ifstream file;
    file.open(filename, ios::in);
    string line;
    while (getline(file, line)) {
        double ping = stod(line);
        pingResults.push_back(ping);
    }
    file.close();
    return pingResults;
}

void pimpData(const string &path) {
    std::ifstream src("../Datenanpasser.py", std::ios::binary);
    std::ofstream dst(path + "/Datenanpasser.py", std::ios::binary);
    dst << src.rdbuf();
    dst.close();
    src.close();
    string pimpCommand = "python3 " + path + "/Datenanpasser.py";
    system(pimpCommand.c_str());
}

int main(int argc, char **argv) {
    if (argc < 5 && argc != 2) {
        cout << "PcapReader [client.pcap] [server.pcap] [serverIp] [locale clientIp] [global clientIp]" << endl;
        cout << "PcapReader [client.pcap] [server.pcap] [serverIp] [locale clientIp] [global clientIp] -m" << endl;
        cout << "PcapReader [path to pcaps] [serverIp] [locale clientIp] [global clientIp]" << endl;
        cout << "PcapReader [path to pcaps] [serverIp] [locale clientIp] [global clientIp] -p" << endl;
        cout << "PcapReader [path to pcaps]" << endl;
        cout << "PcapReader [path to filesystem] -r (noch nicht implementiert)" << endl;
        return -1;
    }
    string clientFilename;
    string serverFilename;
    string serverIp;
    string localeClientIp;
    string globalClientIp;

    if(argc == 7){
        string arg6 = argv[6];
        if(arg6 == "-m"){
            /*
             * Zwei einzelne Pcaps vom Server und Client aus MobileMessung vergleichen
             */
            clientFilename = argv[1];
            serverFilename = argv[2];
            serverIp = argv[3];
            localeClientIp = argv[4];
            globalClientIp = argv[5];

            struct pcapValues clientValues = readMobilePcapFile(clientFilename, localeClientIp, serverIp);
            struct pcapValues serverValues = readMobilePcapFile(serverFilename, serverIp, globalClientIp);

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

            writeResultToFile(downloadResult);
        }
    } else if (argc == 6) {
        string arg5 = argv[5];
        if (arg5 == "-p") {
            string path = argv[1];
            serverIp = argv[2];
            string clientIpLocal = argv[3];
            string clientIpGlobal = argv[4];
            clientFilename = path + "/pingClient.pcap";
            serverFilename = path + "/pingServer.pcap";
            string pinglogFilename = path + "/ping.txt";

            vector<struct timeval> clientValues = readPingFile(clientFilename, clientIpLocal, serverIp);
            vector<struct timeval> serverValues = readPingFile(serverFilename, serverIp, clientIpGlobal);

            vector<double> delays = getPingResults(clientValues, serverValues);
            vector<double> pings = readPingLog(pinglogFilename);
            vector<double> results;
            if (delays.size() == pings.size()) {
                for (int i = 0; i < delays.size(); i++) {
                    results.push_back(delays[i] - pings[i]);
                }
            } else {
                cerr << "ERROR pcaps results: " << delays.size() << " ping results: " << pings.size() << endl;
            }

            writeDelayFile(path + "/result.csv", results);
        } else {
            /*
             * Zwei einzelne Pcaps vom Server und Client vergleichen
             */
            clientFilename = argv[1];
            serverFilename = argv[2];
            serverIp = argv[3];
            localeClientIp = argv[4];
            globalClientIp = arg5;

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

            writeResultToFile(uploadResult, downloadResult);
        }
    } else if (argc == 5) {
        string path = argv[1];
        serverIp = argv[2];
        localeClientIp = argv[3];
        globalClientIp = argv[4];
        string bwDownloadJson = path + "/download.json";
        string bwUploadJson = path + "/upload.json";
        string bandwithParserCall = "python ../BandwithParser.py " + path;
        system(bandwithParserCall.c_str());
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

            string resultPath = path + "/Ergebnis/" + cbrMode + "/";
            writeResultToFile(resultPath, uploadResult, downloadResult);
            pimpData(path);
        }
    } else if (argc == 2) {
        string path = argv[1];
        serverIp = "131.173.33.228";
        localeClientIp = "192.168.8.100";
        string globalClientIpFile = path + "/globalIp";
        ifstream globalIpFile;
        globalIpFile.open(globalClientIpFile);

        globalIpFile >> globalClientIp;
        string bwDownloadJson = path + "/download.json";
        string bwUploadJson = path + "/upload.json";
        string bandwithParserCall = "python ../BandwithParser.py " + path;
        system(bandwithParserCall.c_str());
        for (int i = 0; i < 2; i++) {
            string cbrMode = "fast";
            if (i == 0) {
                cbrMode = "slow";
            }
            cout << endl << "CBR-Mode: " << cbrMode << endl;
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

            cout << endl << "starting Upload" << endl;
            struct result uploadResult = getResults(uploadValues);
            cout << endl << "starting Download" << endl;
            struct result downloadResult = getResults(downloadValues);

            string resultPath = path + "/Ergebnis/" + cbrMode + "/";
            writeResultToFile(resultPath, uploadResult, downloadResult);
            pimpData(path);
        }
    }
    return 0;
}