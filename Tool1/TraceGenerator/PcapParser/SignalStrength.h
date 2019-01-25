//
// Created by drieke on 25.01.19.
//

#ifndef TRACEGENERATOR_SIGNALSTRENGTH_H
#define TRACEGENERATOR_SIGNALSTRENGTH_H

#include <string>

using namespace std;

struct signalStrength{
    unsigned long ts;
    int sigStr;
    string type;
};

#endif //TRACEGENERATOR_SIGNALSTRENGTH_H
