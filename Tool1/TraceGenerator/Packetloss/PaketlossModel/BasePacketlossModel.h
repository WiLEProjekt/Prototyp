
#ifndef TRACEGENERATOR_PAKETLOSSMODEL_H
#define TRACEGENERATOR_PAKETLOSSMODEL_H

#include <string>
#include <random>
using namespace std;

class BasePacketlossModel {
protected:
    mt19937 randomGenerator; //Mersenne Twister Engine
    uniform_real_distribution<float> distribution; //Uniform floats in [a,b) => a=0.0, b=1.0 is not 100% correct. Correct would be [0.0, 1.0]. Now its [0.0, 1.0). Error is neglectable
    long numPackets;

    /**
     * Generates a random number
     * @return a random number
     */
    float generateRandomNumber();

    /**
     * Constructor
     * @param numPackets number of packets
     */
    explicit BasePacketlossModel(long numPackets) : numPackets(numPackets) {
        randomGenerator.seed(time(0));
        //randomGenerator.seed(1);
        auto *dist = new uniform_real_distribution<float>(0.0, 1.0);
        distribution = *dist;
        delete (dist);
    }

    /**
     * Constructor
     * @param seed seed of the random generator
     * @param numPackets number of packets
     */
    BasePacketlossModel(unsigned int seed, long numPackets) : numPackets(numPackets) {
        randomGenerator.seed(seed);
        auto *dist = new uniform_real_distribution<float>(0.0, 1.0);
        distribution = *dist;
        delete (dist);
    }

public:
    /**
     * Builds a loss trace
     * @return a loss trace
     */
    virtual vector<bool> buildTrace() = 0;

    void calculateBursts(vector<bool> &trace, int &i, int &losscounter, int &receivecounter,
                         int &temp, vector<int> &genSizes);
};


#endif //TRACEGENERATOR_PAKETLOSSMODEL_H
