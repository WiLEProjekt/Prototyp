
#include <cstring>
#include <algorithm>
#include "TraceGenerator.h"
#include "PaketlossModel/MarkovModel.h"
#include "PaketlossModel/GilbertElliot.h"
#include "TraceSaver.h"
#include <random>

TraceGenerator::TraceGenerator(int argc, char **argv) {
    if (argc < 2) {
        this->printError();
        return;
    }
    if (argc == 2 && strcmp(argv[1], "-showmodel") == 0) {
        this->printModels();
        return;
    } else {
        string modelname(argv[1]);
        std::transform(modelname.begin(), modelname.end(), modelname.begin(), ::tolower);

        PacketlossModel *model;
        unsigned int seed = static_cast<unsigned int>(atol(argv[2]));
        long numPackets = atol(argv[3]);

        if (strcmp(modelname.c_str(), "-real") == 0) {

        } else if (strcmp(modelname.c_str(), "markov") == 0) {
            if (argc != 10) {
                this->printModels();
            } else {
                float p12 = atof(argv[4]);
                float p14 = atof(argv[5]);
                float p23 = atof(argv[6]);
                float p31 = atof(argv[7]);
                float p32 = atof(argv[8]);
                float p41 = atof(argv[9]);
                model = new MarkovModel(seed, numPackets, p12, p14, p23, p31, p32, p41);
            }
        } else if (strcmp(modelname.c_str(), "gilbertelliot") == 0) {

            if (argc != 8) {
                this->printModels();
            } else {
                float p = atof(argv[4]);
                float r = atof(argv[5]);
                float k = atof(argv[6]);
                float h = atof(argv[7]);
                if (numPackets < 1 || numPackets > numeric_limits<long>::max() - 1 || p < 0 || p > 1 || r < 0 ||
                    r > 1 || k < 0 || k > 1 || h < 0 || h > 1) {
                    this->printModels();
                } else {
                    model = new GilbertElliot(seed, numPackets, p, r, k, h);
                }
            }

        } else if (strcmp(modelname.c_str(), "gilbert") == 0) {
            if (argc != 7) {
                this->printModels();
            } else {
                float p = atof(argv[4]);
                float r = atof(argv[5]);
                float k = 1;
                float h = atof(argv[6]);
                if (numPackets < 1 || numPackets > numeric_limits<long>::max() - 1 || p < 0 || p > 1 || r < 0 ||
                    r > 1 || k < 0 || k > 1 || h < 0 || h > 1) {
                    this->printModels();
                } else {
                    model = new GilbertElliot(seed, numPackets, p, r, k, h);
                }
            }

        } else if (strcmp(modelname.c_str(), "simplegilbert") == 0) {
            if (argc != 6) {
                this->printModels();
            } else {
                float p = atof(argv[4]);
                float r = atof(argv[5]);
                float k = 1;
                float h = 0;
                if (numPackets < 1 || numPackets > numeric_limits<long>::max() - 1 || p < 0 || p > 1 || r < 0 ||
                    r > 1 || k < 0 || k > 1 || h < 0 || h > 1) {
                    this->printModels();
                } else {
                    model = new GilbertElliot(seed, numPackets, p, r, k, h);
                }
            }

        } else if (strcmp(modelname.c_str(), "bernoulli") == 0) {
            if (argc != 5) {
                this->printModels();
            } else {
                float p = atof(argv[4]);
                float r = 1 - p;
                float k = 1;
                float h = 0;
                if (numPackets < 1 || numPackets > numeric_limits<long>::max() - 1 || p < 0 || p > 1 || r < 0 ||
                    r > 1 || k < 0 || k > 1 || h < 0 || h > 1) {
                    this->printModels();
                } else {
                    model = new GilbertElliot(seed, numPackets, p, r, k, h);
                }
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
            "arguments [args]\n\tTraceGenerator -showmodel\tshows all Models\n\tTraceGenerator -real [trace-file]"
         << endl;
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


