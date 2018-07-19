#include <fstream>
#include <iostream>
#include "PacketLossToParameterParser.h"
#include "GilbertElliotParser.h"
#include "SimpleGilbertParser.h"
#include "BernoulliParser.h"
#include "GilbertParser.h"
#include "MarkovParser.h"
#include <algorithm>
#include <map>

PacketLossToParameterParser::PacketLossToParameterParser(PacketLossModelType packetLossModel, string filename) {
    this->trace = this->readFile(filename);
    this->packetLossModel = packetLossModel;
    if (trace.empty()) {
        cout << "file " << filename << " not found or is empty" << endl;
        throw invalid_argument("file " + filename + " not found or is empty");
    }
}

ExtractParameter PacketLossToParameterParser::parseParameter(unsigned int gMin) {
    float *parameter;
    switch (packetLossModel) {
        case BERNOULLI:
            parameter = BernoulliParser().parseParameter(trace);
            break;
        case SIMPLE_GILBERT:
            parameter = SimpleGilbertParser().parseParameter(trace);
            break;
        case GILBERT:
            parameter = GilbertParser().parseParameter(trace);
            break;
        case GILBERT_ELLIOT:
            parameter = GilbertElliotParser().parseParameter(trace, gMin);
            break;
        case MARKOV:
            parameter = MarkovParser().parseParameter(trace, gMin);
            break;
        default:
            cout << "invalid model: " << packetLossModel << endl;
            throw invalid_argument("invalid model: " + packetLossModel);
    }
    ExtractParameter extractParameter{};
    extractParameter.parameter = parameter;
    extractParameter.packetCount = trace.size();
    return extractParameter;
}

ExtractParameter PacketLossToParameterParser::parseParameter() {
    this->parseParameter(0);
}

vector<bool> PacketLossToParameterParser::readFile(string filename) {
    vector<bool> trace;
    ifstream file;
    file.open(filename, ios::in);
    char c;
    while (file.get(c)) {
        trace.push_back(c != '0');
    }
    trace.pop_back();
    return trace;
}
