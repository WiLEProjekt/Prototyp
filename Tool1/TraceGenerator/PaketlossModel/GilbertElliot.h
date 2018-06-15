

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
    /**
     * Constructor
     * @param seed the seed of the random generator
     * @param numPackets the number of packets
     * @param p chance of swithcing to burst-state
     * @param r chance of switching to gap-state
     * @param k chance of loss in gap-state
     * @param h chance of success in burst-state
     */
    GilbertElliot(unsigned int seed, long numPackets, float p, float r, float k, float h)
            : PacketlossModel(seed, numPackets), p(p), r(r), k(k), h(h) {};

    GilbertElliot(unsigned int seed, long numPackets, float parameter[])
            : PacketlossModel(seed, numPackets), p(parameter[0]), r(parameter[1]), k(parameter[2]), h(parameter[3]) {};
};


#endif //TRACEGENERATOR_BERNOULLIMODEL_H
