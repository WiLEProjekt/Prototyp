#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

unsigned long counter = 0;

unsigned int parseSequenzNumber(string line){;
    string delimeterFront = "icmp_seq=";
    string delimeterBack = " ttl";
    size_t pos = line.find(delimeterFront);
    line.erase(0, pos + delimeterFront.length());
    pos = line.find(delimeterBack);
    string token = line.substr(0, pos);
    cout << token << endl;
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

void write(bool packetRecieved){
    counter++;
    fstream file;
    file.open("trace.txt", ios::app);
    file << packetRecieved;
    file.close();
}

int main(int argc, char** argv) {
    const unsigned int MAX_SEQ_NUM = 65536;
    unsigned long packetNumber;

    if(argc < 2){
        cout << "Pingparser [filename]" << endl;
        return -1;
    }
    char* filename = argv[1];

    fstream fileStream;
    fileStream.open(filename, ios::in);
    unsigned int lastSeqNum = 0;
    for(string line; getline(fileStream, line);){
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
        }
    }

    fileStream.close();
    return 0;
}