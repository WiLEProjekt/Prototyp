
#include "TraceGenerator.h"

vector<bool> TraceGenerator::generateTrace(PacketLossModelType modelType, unsigned long numPackets, unsigned int seed,
                                           double *params) {
    BasePacketlossModel *model = nullptr;
    if (seed == 0) {
        seed = time(0);
    }

    if (modelType == MARKOV) {
        model = new MarkovModel(numPackets, seed, params);
    } else if (modelType == GILBERT_ELLIOT || modelType == GILBERT || modelType == SIMPLE_GILBERT ||
               modelType == BERNOULLI) {
        model = new GilbertElliotModel(numPackets, seed, params);
    } else {
        cout << "No valid model: " << modelType << endl;
        this->printModels();
        vector<bool> result;
        return result;
    }

    vector<bool> trace = model->buildTrace();
    this->printPacketloss(trace);
    delete (model);
    return trace;
}

void TraceGenerator::printModels() {
    cout << "Tracegenerator [outputfile]\n"
         << "\tGilbertElliot\t<seed [1-" << numeric_limits<unsigned int>::max() << "]> <number_of_packets [1-"
         << numeric_limits<long>::max() << "]> <param p [0-1]> <param r [0-1]> <param k [0-1]> <param h [0-1]>\n"
         << "\tGilbert\t\t\t<seed [1-" << numeric_limits<unsigned int>::max() << "]> <number_of_packets [1-"
         << numeric_limits<long>::max() << "]> <param p [0-1]> <param r [0-1]> <param h [0-1]>\n"
         << "\tSimpleGilbert\t<seed [1-" << numeric_limits<unsigned int>::max() << "]> <number_of_packets [1-"
         << numeric_limits<long>::max() << "]> <param p [0-1]> <param r [0-1]\n"
         << "\tBernoulli\t\t<seed [1-" << numeric_limits<unsigned int>::max() << "]> <number_of_packets [1-"
         << numeric_limits<long>::max() << "]> <param p [0-1]>\n"
         << "\tMarkov\t\t\t<seed [1-" << numeric_limits<unsigned int>::max() << "]> <number_of_packets [1-"
         << numeric_limits<long>::max() << "]> <p13> <p31> <p32> <p23> <p14>" << endl;
}

void TraceGenerator::printPacketloss(vector<bool> trace) {
    long zeros = 0;
    for (auto &&i : trace) {
        if (i == false) {
            zeros++;
        }
    }
    cout << " Packetloss: " << (100 / (float) trace.size()) * (float) zeros << "%" << endl;
}