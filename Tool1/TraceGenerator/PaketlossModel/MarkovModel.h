
#ifndef TRACEGENERATOR_MARKOVMODEL_H
#define TRACEGENERATOR_MARKOVMODEL_H


#include <vector>
#include "BasePacketlossModel.h"

using namespace std;

class MarkovModel : public BasePacketlossModel {
protected:
    vector<bool> buildTrace() override;

private:
    float p13, p14, p23, p31, p32, p41;

public:
    /**
     *
     * Constructor
     * @param numPackets number of packets
     * @param the parameters [p13, p31, p32, p23, p14]
     */
    MarkovModel(long numPackets, const float parameter[]) : BasePacketlossModel(numPackets) {
        p13 = parameter[0];
        p31 = parameter[1];
        p32 = parameter[2];
        p23 = parameter[3];
        p14 = parameter[4];
        p41 = 1;
    }

    MarkovModel(long numPackets, unsigned int seed, const float parameter[]) : BasePacketlossModel(seed, numPackets) {
        p13 = parameter[0];
        p31 = parameter[1];
        p32 = parameter[2];
        p23 = parameter[3];
        p14 = parameter[4];
        p41 = 1;
    }

    /**
     *
     * Constructor
     * @param numPackets number of packets
     * @param p13 chance of switching from state-1 to state-3
     * @param p14 chance of switching from state-1 to state-4
     * @param p23 chance of switching from state-2 to state-3
     * @param p31 chance of switching from state-3 to state-1
     * @param p32 chance of switching from state-3 to state-2
     * @param p41 chance of switching from state-4 to state-1
     */
    MarkovModel(long numPackets, float p13, float p31, float p32, float p23, float p14)
            : BasePacketlossModel(numPackets), p13(p13), p14(p14), p23(p23), p31(p31), p32(p32), p41(1) {}
    /**
     *
     * Constructor
     * @param seed seed of the random generator
     * @param numPackets number of packets
     * @param p13 chance of switching from state-1 to state-3
     * @param p14 chance of switching from state-1 to state-4
     * @param p23 chance of switching from state-2 to state-3
     * @param p31 chance of switching from state-3 to state-1
     * @param p32 chance of switching from state-3 to state-2
     * @param p41 chance of switching from state-4 to state-1
     */
    MarkovModel(unsigned int seed, long numPackets, float p13, float p31, float p32, float p23, float p14)
            : BasePacketlossModel(seed, numPackets), p13(p13), p14(p14), p23(p23), p31(p31), p32(p32), p41(1) {}

};


#endif //TRACEGENERATOR_MARKOVMODEL_H
