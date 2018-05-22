//
// Created by woors on 22.05.2018.
//

#ifndef TRACEGENERATOR_PAKETLOSSMODEL_H
#define TRACEGENERATOR_PAKETLOSSMODEL_H

#include "PaketlossModelType.h"
#include <string>

using namespace std;
class PacketlossModel {
public:
    string generateTrace();
    virtual string buildTrace() = 0;
    bool checkTrace(string trace);
};


#endif //TRACEGENERATOR_PAKETLOSSMODEL_H
