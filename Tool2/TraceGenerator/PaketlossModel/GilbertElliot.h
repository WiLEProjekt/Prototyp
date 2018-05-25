

#ifndef TRACEGENERATOR_BERNOULLIMODEL_H
#define TRACEGENERATOR_BERNOULLIMODEL_H


#include <vector>
#include "PacketlossModel.h"

class GilbertElliot : public PacketlossModel{
private:
    float p, r, k, h;
protected:
    vector<bool> buildTrace() override ;

public:
    GilbertElliot(unsigned int seed, long numPackets, float p, float r, float k, float h)
            : PacketlossModel(seed, numPackets), p(p), r(r), k(k), h(h) {};
};


#endif //TRACEGENERATOR_BERNOULLIMODEL_H
