
#ifndef TRACEGENERATOR_TRACEGENERATOR_H
#define TRACEGENERATOR_TRACEGENERATOR_H

#include <iostream>
#include <vector>
#include <stdlib.h>
#include "Packetloss/PaketlossModel/MarkovModel.h"
#include "Packetloss/PaketlossModel/GilbertElliotModel.h"
#include "Packetloss/PacketLossModelType.h"
#include <random>
#include <cstring>
#include <algorithm>


using namespace std;

class TraceGenerator {
private:

    /**
     * Prints the available models on the console
     */
    void printModels();

    /**
     * Prints the packetloss in percent on the console
     * @param trace the loss trace
     */
    void printPacketloss(vector<bool> *trace);


public:

    vector<bool> *
    generateTrace(PacketLossModelType modelType, unsigned long numPackets, unsigned int seed, double *params);
};


#endif //TRACEGENERATOR_TRACEGENERATOR_H
