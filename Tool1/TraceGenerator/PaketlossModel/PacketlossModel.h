
#ifndef TRACEGENERATOR_PAKETLOSSMODEL_H
#define TRACEGENERATOR_PAKETLOSSMODEL_H

#include "PaketlossModelType.h"
#include <string>

using namespace std;
class PacketlossModel {
protected:
    long numPackets;
    float generateRandomNumber();
    PacketlossModel(unsigned int seed, long numPackets):numPackets(numPackets){srand(seed);}
public:
    virtual vector<bool> buildTrace() = 0;
};


#endif //TRACEGENERATOR_PAKETLOSSMODEL_H
