

#ifndef TRACEGENERATOR_BERNOULLIMODEL_H
#define TRACEGENERATOR_BERNOULLIMODEL_H


#include <vector>
#include "BasePacketlossModel.h"
#include <string>
#include <stdexcept>

using namespace std;

class GilbertElliot : public BasePacketlossModel {
private:
    float p = -1;
    float r = -1;
    float k = -1;
    float h = -1;

    string checkParameter();

protected:
    vector<bool> buildTrace() override ;

public:


    GilbertElliot(long numPackets, float p, float r, float k, float h) : BasePacketlossModel(numPackets) {
        this->p = p;
        this->r = r;
        this->k = k;
        this->h = h;
        string invalidArgument = this->checkParameter();
        if (!invalidArgument.empty()) {
            throw invalid_argument("invalid Argument {" + invalidArgument + "}");
        }
    }

    GilbertElliot(long numPackets, float parameter[])
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
     * @param seed the seed of the random generator
     * @param numPackets the number of packets
     * @param p chance of swithcing to burst-state
     * @param r chance of switching to gap-state
     * @param k chance of loss in gap-state
     * @param h chance of success in burst-state
     */
    GilbertElliot(unsigned int seed, long numPackets, float p, float r, float k, float h)
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

    GilbertElliot(unsigned int seed, long numPackets, float parameter[])
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
