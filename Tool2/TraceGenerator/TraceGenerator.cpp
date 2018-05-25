//
// Created by woors on 22.05.2018.
//

#include <cstring>
#include <algorithm>
#include "TraceGenerator.h"
#include "PaketlossModel/MarkovModel.h"
#include "PaketlossModel/GilbertElliot.h"
#include "TraceSaver.h"

TraceGenerator::TraceGenerator(int argc, char **argv) {
    if (argc < 2) {
        this->printError();
    }
    if (argc == 2 && strcmp(argv[1], "-showmodel") == 0) {
        this->printModels();
    } else {
        string modelname(argv[1]);
        std::transform(modelname.begin(), modelname.end(), modelname.begin(), ::tolower);

        PacketlossModel *model;
        unsigned int seed = static_cast<unsigned int>(atol(argv[2]));
        long numPackets = atol(argv[3]);

        if (strcmp(modelname.c_str(), "markov") == 0) {
            model = new MarkovModel(seed, numPackets);
        } else if (strcmp(modelname.c_str(), "gilbertelliot") == 0) {

            if (argc != 8) {
                this->printModels();
            }
            double p = atof(argv[4]);
            double r = atof(argv[5]);
            double k = atof(argv[6]);
            double h = atof(argv[7]);
            if (numPackets < 1 || numPackets > 2147483647 || p < 0 || p > 1 || r < 0 || r > 1 || k < 0 || k > 1 ||
                h < 0 || h > 1) {
                this->printModels();
            } else {
                model = new GilbertElliot(seed, numPackets, p, r, k, h);
            }
        } else if (strcmp(modelname.c_str(), "gilbert") == 0) {
            double p = atof(argv[4]);
            double r = atof(argv[5]);
            double k = 1;
            double h = atof(argv[6]);
            if (numPackets < 1 || numPackets > 2147483647 || p < 0 || p > 1 || r < 0 || r > 1 || k < 0 || k > 1 ||
                h < 0 || h > 1) {
                this->printModels();
            } else {
                model = new GilbertElliot(seed, numPackets, p, r, k, h);
            }
        } else if (strcmp(modelname.c_str(), "simplegilbert") == 0) {
            double p = atof(argv[4]);
            double r = atof(argv[5]);
            double k = 1;
            double h = 0;
            if (numPackets < 1 || numPackets > 2147483647 || p < 0 || p > 1 || r < 0 || r > 1 || k < 0 || k > 1 ||
                h < 0 || h > 1) {
                this->printModels();
            } else {
                model = new GilbertElliot(seed, numPackets, p, r, k, h);
            }
        } else if (strcmp(modelname.c_str(), "bernoulli") == 0) {
            double p = atof(argv[4]);
            double r = 1 - p;
            double k = 1;
            double h = 0;
            if (numPackets < 1 || numPackets > 2147483647 || p < 0 || p > 1 || r < 0 || r > 1 || k < 0 || k > 1 ||
                h < 0 || h > 1) {
                this->printModels();
            } else {
                model = new GilbertElliot(seed, numPackets, p, r, k, h);
            }
        } else {
            cout << "No valid model: " << modelname << endl;
            this->printModels();
            return;
        }

        vector<bool> trace = model->buildTrace();
        this->printPacketloss(trace);
        TraceSaver::writeTraceToFile(trace);
    }
}

void TraceGenerator::printError() {
    cout << "\tTraceGenerator [model] [args...]\tgenerates a trace with Model [model] and "
            "arguments [args]\n\tTraceGenerator -showmodel\tshows all Models" << endl;
}

void TraceGenerator::printModels() {
    cout << "Models:\n\tGilbertElliot\t<seed [1-" << numeric_limits<unsigned int>::max() << "]> <number_of_packets [1-"
         << numeric_limits<long>::max()
         << "]> <param p [0-1]> <param r [0-1]> <param k [0-1]> <param h [0-1]>\n\tGilbert\t\t\t<seed [1-"
         << numeric_limits<unsigned int>::max() << "]> <number_of_packets [1-" << numeric_limits<long>::max()
         << "]> <param p [0-1]> <param r [0-1]> <param h [0-1]>\n\tSimpleGilbert\t<seed [1-"
         << numeric_limits<unsigned int>::max()
         << "]> <number_of_packets [1-" << numeric_limits<long>::max()
         << "]> <param p [0-1]> <param r [0-1]\n\tBernoulli\t\t<seed [1-"
         << numeric_limits<unsigned int>::max() << "]> <number_of_packets [1-" << numeric_limits<long>::max()
         << "]> <param p [0-1]>\n\tMarkov\t\t\t<seed [1-" << numeric_limits<unsigned int>::max()
         << "]> <number_of_packets [1-" << numeric_limits<long>::max() << "]>" << endl;
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


