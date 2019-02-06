
//
// Created by drieke on 25.01.19.
//

#ifndef TRACEGENERATOR_RESULTPOINT_H
#define TRACEGENERATOR_RESULTPOINT_H

#include <map>

using namespace std;

struct resultPoint {
    uint64_t recievedTs;
    int64_t delay;
    bool packetRecieved;
    unsigned long seqNum;
};

#endif //TRACEGENERATOR_RESULTPOINT_H