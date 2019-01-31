#ifndef TRACEGENERATOR_PACKETLOSSTOPARAMETERPARSER_H
#define TRACEGENERATOR_PACKETLOSSTOPARAMETERPARSER_H

#include <vector>
#include <string>
#include "../PacketLossModelType.h"

using namespace std;

struct ExtractParameter {
    double *parameter;
    unsigned long packetCount;
};

class PacketLossToParameterParser {
private:
    PacketLossModelType packetLossModel;
    vector<bool> trace;

    /**
     * reads a 01-trace-file
     * @param filename the filename
     * @return the 01-trace
     */
    vector<bool> readFile(string filename);

public:
    /**
     * Constructor
     * @param packetLossModel the packetloss-model
     * @param filename the input filename
     */
    PacketLossToParameterParser(PacketLossModelType packetLossModel, string filename);

    /**
     * Constructor
     * @param packetLossModelType the packetloss-model
     * @param trace the trace
     */
    PacketLossToParameterParser(PacketLossModelType packetLossModelType, vector<bool> trace) : packetLossModel(
            packetLossModelType), trace(trace) {};

    /**
     * Parses the parameter from the 01-trace
     * @return the paramter
     */
    ExtractParameter parseParameter();

    /**
     * Parses the paramter from the 01-trace with burst-size for Gilbert-Elliot or 4-state-Markov
     * @param gMin the burst size
     * @return the parameter
     */
    ExtractParameter parseParameter(unsigned int gMin);
};


#endif //TRACEGENERATOR_PACKETLOSSTOPARAMETERPARSER_H
