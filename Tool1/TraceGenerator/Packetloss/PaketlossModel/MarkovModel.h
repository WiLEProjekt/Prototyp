#ifndef TRACEGENERATOR_MARKOVMODEL_H
#define TRACEGENERATOR_MARKOVMODEL_H

#include <vector>
#include "BasePacketlossModel.h"

using namespace std;

class MarkovModel : public BasePacketlossModel {
protected:
    vector<bool> buildTrace() override;

private:
    double p13, p14, p23, p31, p32, p41 = 1;

public:
    /**
     *
     * Constructor
     * @param numPackets number of packets
     * @param the parameters [p13, p31, p32, p23, p14]
     */
    MarkovModel(long numPackets, unsigned int seed, const double parameter[]) : BasePacketlossModel(seed, numPackets) {
        p13 = parameter[0];
        p31 = parameter[1];
        p32 = parameter[2];
        p23 = parameter[3];
        p14 = parameter[4];
        p41 = 1;
    }
};


#endif //TRACEGENERATOR_MARKOVMODEL_H
