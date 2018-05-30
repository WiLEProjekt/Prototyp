#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

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

void evaluate(unsigned long packetNumber, vector<vector<unsigned int>*> iterations){
    cout << packetNumber << " Packets in " << iterations.size() << " iterations" << endl;
}

int main(int argc, char** argv) {
    vector<vector<unsigned int>*> iterations;
    auto *seqNums = new vector<unsigned int>;
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
            seqNums->push_back(seqNum);
            if(seqNum < lastSeqNum){
                iterations.push_back(seqNums);
                seqNums = new vector<unsigned int>;
            }
            lastSeqNum = seqNum;
        } else if(line.find("packets transmitted, ") != string::npos){
            iterations.push_back(seqNums);
            packetNumber = parsePacketNumber(line);
        }
    }

    fileStream.close();

    evaluate(packetNumber, iterations);
    return 0;
}