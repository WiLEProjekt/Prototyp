#ifndef TRACEGENERATOR_PACKETLOSSTOPARAMETERPARSER_H
#define TRACEGENERATOR_PACKETLOSSTOPARAMETERPARSER_H

#include <vector>
#include <string>

using namespace std;

enum PacketLossModelType {
    BERNOULLI,
    SIMPLE_GILBERT,
    GILBERT,
    GILBERT_ELLIOT,
    MARKOV
};

struct ExtractParameter {
    float *parameter;
    unsigned long packetCount;
};

class PacketLossToParameterParser {
private:
    PacketLossModelType packetLossModel;
    vector<bool> trace;

    /**
     * Parses a 01-trace to parameter for a 4-state-markov model
     * @param trace the 01-trace
     * @param gMin the burst-size
     * @return the parameter
     */
    float *parseMarkov(vector<bool> trace, unsigned int gMin);

    /**
     * Parses a 01-trace to parameter for a Bernoulli model
     * @param trace the 01-trace
     * @return the parameter
     */
    float *parseBernoulli(vector<bool> trace);

    /**
     * Parses a 01-trace to parameter for a Simple Gilbert model
     * @param trace the 01-trace
     * @return the paramter
     */
    float *parseSimpleGilbert(vector<bool> trace);

    /**
     * Parses a 01-trace to parameter for a Gilbert model
     * @param trace the 01-trace
     * @return the paramter
     */
    float *parseGilbert(vector<bool> trace);

    /**
     * Parses a 01-trace to parameter for a Gilbert-Elliot model
     * @param trace the 01-trace
     * @param gMin the burst-size
     * @return the parameter
     */
    float *parseGilbertElliot(vector<bool> trace, unsigned int gMin);

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
