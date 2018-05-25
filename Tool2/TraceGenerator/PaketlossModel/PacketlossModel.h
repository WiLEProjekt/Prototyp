//
// Created by woors on 22.05.2018.
//

#ifndef TRACEGENERATOR_PAKETLOSSMODEL_H
#define TRACEGENERATOR_PAKETLOSSMODEL_H

#include "PaketlossModelType.h"
#include <string>

using namespace std;
class PacketlossModel {
protected:
    float generateRandomNumber();
    PacketlossModel(unsigned int seed){srand(seed);}
public:
    virtual vector<bool> buildTrace() = 0;
};


#endif //TRACEGENERATOR_PAKETLOSSMODEL_H
