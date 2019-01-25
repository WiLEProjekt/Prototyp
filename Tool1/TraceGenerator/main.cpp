#include "TraceGenerator.h"
#include "PcapParser/PcapParser.h"


void writeDelayFile(const string &filename, vector<int64_t> delays) {
    ofstream uploadDelayFile;
    uploadDelayFile.open(filename);
    for (int64_t ts: delays) {
        uploadDelayFile << ts << " ";
    }
    uploadDelayFile.flush();
    uploadDelayFile.close();
}

void writeFullTraceFile(const string &filename, vector<resultPoint> result) {
    ofstream uploadDelayFile;
    uploadDelayFile.open(filename);

    for (struct resultPoint rp : result) {
        uploadDelayFile << rp.recievedTs << ";" << rp.delay << ";" << rp.packetRecieved << ";" << rp.seqNum << ";"
                        << rp.sigStrength << ";" << rp.type << endl;
    }
    uploadDelayFile.flush();
    uploadDelayFile.close();
}

void writeResultToFile(const result &download) {
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


/**
 * Mögliche aufrufe:
 * clientTrace serverTrace globalClientIp
 * clientTrace serverTrace globalClientIp paramflag
 * gen [model] [params]
 */
int main(int argc, char** argv) {
    string params[argc];
    for (int i = 0; i < argc; i++) {
        params[i] = argv[i];
    }

    if(params[1] == "gen"){
        //TODO Loss generieren und abspeichern
    } else {
        string clientTrace = params[1];
        string serverTrace = params[2];
        string globalClientIp = params[3];
        bool paramized = false;
        if(argc >= 5){
            paramized = true;
        }
        PcapParser parser;
        struct result pcapResult = parser.startParsing(clientTrace, serverTrace, globalClientIp);
        if(paramized){
            //TODO params generieren und loss generieren
        }
        writeResultToFile(pcapResult);
    }

    return 0;
}