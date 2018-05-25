//
// Created by drieke on 25.05.18.
//

#ifndef TRACEGENERATOR_BERNOULLIMODEL_H
#define TRACEGENERATOR_BERNOULLIMODEL_H


#include <vector>
#include "PacketlossModel.h"

class GilbertElliot : public PacketlossModel{
private:
    double p;
    double r;
    double k;
    double h;
protected:
    vector<bool> buildTrace() override ;

public:
    GilbertElliot(unsigned int seed, long numPackets, double p, double r, double k, double h)
            : PacketlossModel(seed, numPackets), p(p), r(r), k(k), h(h) {};
};


#endif //TRACEGENERATOR_BERNOULLIMODEL_H
