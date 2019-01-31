

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
     * @param p chance of switching to burst-state
     * @param r chance ot switching to gap-state
     * @param k chance of receive in gap-state
     * @param h chance of receive in burst-state
     */
    GilbertElliotModel(long numPackets, double p, double r, double k, double h) : BasePacketlossModel(numPackets) {
        this->p = p;
        this->r = r;
        this->k = k;
        this->h = h;
        string invalidArgument = this->checkParameter();
        if (!invalidArgument.empty()) {
            throw invalid_argument("invalid Argument {" + invalidArgument + "}");
        }
    }

    /**
     * Constructor
     * @param numPackets the number of packets
     * @param parameter the 4 parameter [p, r, k, h]
     */
    GilbertElliotModel(long numPackets, double parameter[])
            : BasePacketlossModel(numPackets) {
        this->p = parameter[0];
        this->r = parameter[1];
        this->k = parameter[2];
        this->h = parameter[3];
        string invalidArgument = this->checkParameter();
        if (!invalidArgument.empty()) {
            throw invalid_argument("invalid Argument {" + invalidArgument + "}");
        }
    }

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

    /**
     * Constructor
     * @param seed the seed of the random generator
     * @param numPackets the number of packets
     * @param p chance of switching to burst-state
     * @param r chance of switching to gap-state
     * @param k chance of receive in gap-state
     * @param h chance of receive in burst-state
     */
    GilbertElliotModel(unsigned int seed, long numPackets, float p, float r, float k, float h)
            : BasePacketlossModel(seed, numPackets) {
        this->p = p;
        this->r = r;
        this->k = k;
        this->h = h;
        string invalidArgument = this->checkParameter();
        if (!invalidArgument.empty()) {
            throw invalid_argument("invalid Argument {" + invalidArgument + "}");
        }
    }

    vector<int> buildTrace2();
};


#endif //TRACEGENERATOR_BERNOULLIMODEL_H
