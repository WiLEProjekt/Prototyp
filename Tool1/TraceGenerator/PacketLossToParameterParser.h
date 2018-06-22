#ifndef TRACEGENERATOR_PACKETLOSSTOPARAMETERPARSER_H
#define TRACEGENERATOR_PACKETLOSSTOPARAMETERPARSER_H

#include <vector>
#include <string>

using namespace std;

enum PacketLossModelType {
    BERNOULI,
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
    const static unsigned int G_MIN_DEFAULT = 4;
    PacketLossModelType packetLossModel;
    string filename;

    float *parseMarkov(vector<bool> trace, unsigned int gMin);

    float *parseBernoulli(vector<bool> trace);

    float *parseSimpleGilbert(vector<bool> trace);

    float *parseGilbert(vector<bool> trace);

    float *parseGilbertElliot(vector<bool> trace, unsigned int gMin);

    vector<bool> readFile(string filename);

public:


    PacketLossToParameterParser(PacketLossModelType packetLossModel, string filename);

    ExtractParameter parseParameter();

    ExtractParameter parseParameter(unsigned int gMin);
};


#endif //TRACEGENERATOR_PACKETLOSSTOPARAMETERPARSER_H
