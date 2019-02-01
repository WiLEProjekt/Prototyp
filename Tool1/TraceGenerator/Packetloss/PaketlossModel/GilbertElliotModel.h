

#ifndef TRACEGENERATOR_BERNOULLIMODEL_H
#define TRACEGENERATOR_BERNOULLIMODEL_H


#include <vector>
#include "BasePacketlossModel.h"
#include <string>
#include <stdexcept>

using namespace std;

class GilbertElliotModel : public BasePacketlossModel {
private:
    double p = -1;
    double r = -1;
    double k = -1;
    double h = -1;

    /**
     * Checks if the parameter are in a valid range
     * @return the name of the invalid parameter or empty string
     */
    string checkParameter();

public:
    vector<bool> buildTrace() override;

    /**
     * Constructor
     * @param numPackets the number of packets
     * @param seed the seed of the random generator
     * @param parameter the 4 parameter [p, r, k, h]
     */
    GilbertElliotModel(long numPackets, unsigned int seed, const double parameter[])
            : BasePacketlossModel(seed, numPackets) {
        this->p = parameter[0];
        this->r = parameter[1];
        this->k = parameter[2];
        this->h = parameter[3];
        string invalidArgument = this->checkParameter();
        if (!invalidArgument.empty()) {
            throw invalid_argument("invalid Argument {" + invalidArgument + "}");
        }
    }
};


#endif //TRACEGENERATOR_BERNOULLIMODEL_H
