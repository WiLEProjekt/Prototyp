#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

unsigned long counter = 0;

unsigned int parseSequenzNumber(string line){;
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

unsigned long parsePacketNumber(string line){
    string delimeter = " packets transmitted, ";
    string token = line.substr(0, line.find(delimeter));
    unsigned long packetNumberValue;
    stringstream(token) >> packetNumberValue;
    return packetNumberValue;
}

int main(int argc, char** argv) {
    const unsigned int MAX_SEQ_NUM = 65536;
    unsigned long packetCounter = 0;

    vector<bool> parsedTrace;
    if(argc < 4){
        cout << "Pingparser [filename of pingtrace] [output-filename .txt] [total number of packets used in the pingtrace]" << endl;
        return -1;
    }
    string inputFilename = argv[1];
    string outputFilename = argv[2];
    unsigned int packetNumber = atoi(argv[3]);

    fstream fileStream;
    fileStream.open(inputFilename, ios::in);
    unsigned int lastSeqNum = 0;
    unsigned int lostPackets = 0;
    for(string line; getline(fileStream, line);){ //Grab all lines
        if(line.find("icmp_seq=") != string::npos){ //search for the sequence number
            unsigned int seqNum = parseSequenzNumber(line); //parse string to int
            int diff = seqNum - lastSeqNum;
            if(diff > 1){ //packets are lost in ascending order, maximum sequencenumber is not yet reached
                for(int i = 0; i<diff-1; i++){
                    packetCounter++;
                    parsedTrace.push_back(false);
                    //write(false);
                }
            }else if(diff <= 0){ //Packet loss at maximum sequencenumber detected
                diff = MAX_SEQ_NUM - lastSeqNum + seqNum + 1;
                for(int i = 0; i<diff-1; i++){
                    packetCounter++;
                    parsedTrace.push_back(false);
                    //write(false);
                }
            }else{ //diff = 1
                packetCounter++;
                parsedTrace.push_back(true);
                //write(true);
            }

            lastSeqNum = seqNum;
        }
    }
    fileStream.close();
    if(packetCounter < packetNumber){ //last packages sent are lost
        for(int i = 0; i< packetNumber-1; i++){
            parsedTrace.push_back(false);
            //write(false);
        }
    }
    cout << "Verarbeitete Pakete: " << packetNumber << endl;
    string out = "../ParsedTraces/"+outputFilename;
    fstream file;
    file.open(out, ios::out);
    for(int i = 0; i<parsedTrace.size(); i++){
        file << parsedTrace[i];
    }
    //file << packetRecieved;
    file.close();

    return 0;
}

/*
       if(line.find("icmp_seq=") != string::npos) {
           unsigned int seqNum = parseSequenzNumber(line);
           if(seqNum < lastSeqNum){
               unsigned int loss = MAX_SEQ_NUM - lastSeqNum + seqNum - 1;
               if(loss == 0){
                   write(true);
               } else {
                   for (int i = 0; i < loss; ++i) {
                       write(false);
                   }
               }
           } else{
               int loss = seqNum - lastSeqNum - 1;
               if(loss == 0){
                   write(true);
               } else {
                   for (int i = 0; i < loss; i++) {
                       write(false);
                   }
               }
           }
           lastSeqNum = seqNum;
       } else if(line.find("packets transmitted, ") != string::npos){
           packetNumber = parsePacketNumber(line);
           unsigned long loss = packetNumber - counter;
           for(int i = 0; i < loss; i++){
               write(false);
           }
       }*/