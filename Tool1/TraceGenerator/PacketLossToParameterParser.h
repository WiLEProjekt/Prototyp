//
// Created by drieke on 15.06.18.
//

#ifndef TRACEGENERATOR_PACKETLOSSTOPARAMETERPARSER_H
#define TRACEGENERATOR_PACKETLOSSTOPARAMETERPARSER_H

#include <vector>
#include <string>

using namespace std;

enum PacketLossModel {
    BERNOULI,
    SIMPLE_GILBERT,
    GILBERT,
    GILBERT_ELLIOT,
    MARKOV
};

class PacketLossToParameterParser {
private:
    PacketLossModel packetLossModel;
    string filenname;

    float *parseMarkov(vector<bool> trace);

    float *parseBernoulli(vector<bool> trace);

    float *parseSimpleGilbert(vector<bool> trace);

    float *parseGilbert(vector<bool> trace);

    float *parseGilbertElliot(vector<bool> trace);

    vector<bool> readFile(string filename);

public:
    PacketLossToParameterParser(PacketLossModel packetLossModel, string filename);

    float *parseParameter();
};


#endif //TRACEGENERATOR_PACKETLOSSTOPARAMETERPARSER_H
