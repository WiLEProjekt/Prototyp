
#ifndef TRACEGENERATOR_MARKOVMODEL_H
#define TRACEGENERATOR_MARKOVMODEL_H


#include <vector>
#include "PacketlossModel.h"

using namespace std;

class MarkovModel : public PacketlossModel {
protected:
    vector<bool> buildTrace() override;

private:
    float p12, p14, p23, p13, p32, p41;

public:
    MarkovModel(unsigned int seed, long numPackets, float p12, float p14, float p23, float p13, float p32, float p41)
            : PacketlossModel(seed, numPackets), p12(p12), p14(p14), p23(p23), p13(p13), p32(p32), p41(p41) {}
};


#endif //TRACEGENERATOR_MARKOVMODEL_H
