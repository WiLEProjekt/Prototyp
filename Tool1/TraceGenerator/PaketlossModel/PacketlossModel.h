
#ifndef TRACEGENERATOR_PAKETLOSSMODEL_H
#define TRACEGENERATOR_PAKETLOSSMODEL_H

#include "PaketlossModelType.h"
#include <string>
#include <random>
using namespace std;
class PacketlossModel {
protected:
    mt19937 generator; //Mersenne Twister Engine
    uniform_real_distribution<float> distribution; //Uniform floats in [a,b) => a=0.0, b=1.0 is not 100% correct. Correct would be [0.0, 1.0]. Now its [0.0, 1.0). Error is neglectable
    long numPackets;
    float generateRandomNumber();
    PacketlossModel(unsigned int seed, long numPackets):numPackets(numPackets){generator.seed(seed); distribution = *new uniform_real_distribution<float>(0.0, 1.0);}
public:
    virtual vector<bool> buildTrace() = 0;
};


#endif //TRACEGENERATOR_PAKETLOSSMODEL_H
