//
// Created by drieke on 25.01.19.
//

#ifndef TRACEGENERATOR_PCAPVALUES_H
#define TRACEGENERATOR_PCAPVALUES_H

#include <map>
#include <vector>

using namespace std;

struct pcapValues {
    map<unsigned long, struct timeval> *send;
    map<unsigned long, struct timeval> *received;
    vector<unsigned long> *seqNumsSend;
    vector<unsigned long> *seqNumsReceived;
};

#endif //TRACEGENERATOR_PCAPVALUES_H
