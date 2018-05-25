//
// Created by drieke on 25.05.18.
//

#ifndef TRACEGENERATOR_BERNOULLIMODEL_H
#define TRACEGENERATOR_BERNOULLIMODEL_H


#include <vector>
#include "PacketlossModel.h"

class GilbertElliot : public PacketlossModel{
private:
    int numPackets;
    int p;
    int r;
    int k;
    int h;
protected:
    vector<bool> buildTrace() override ;

public:
    GilbertElliot(unsigned int seed, int numPackets, int p, int r, int k, int h)
            : PacketlossModel(seed), numPackets(numPackets), p(p), r(r), k(k), h(h) {};
};


#endif //TRACEGENERATOR_BERNOULLIMODEL_H
