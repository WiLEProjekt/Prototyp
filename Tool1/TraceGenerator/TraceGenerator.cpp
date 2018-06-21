
#include <cstring>
#include <algorithm>
#include "TraceGenerator.h"
#include "PaketlossModel/MarkovModel.h"
#include "PaketlossModel/GilbertElliot.h"
#include "TraceSaver.h"
#include "PacketLossToParameterParser.h"
#include <random>

TraceGenerator::TraceGenerator(int argc, char **argv) {
    if (argc < 2) {
        this->printError();
        return;
    }
    if (argc == 2 && strcmp(argv[1], "-showmodel") == 0) {
        this->printModels();
        return;
    } else if (argc == 4 && strcmp(argv[1], "-extract") == 0) {
        string filename = argv[2];
        string packetlossModelName = argv[3];
        std::transform(packetlossModelName.begin(), packetlossModelName.end(), packetlossModelName.begin(), ::tolower);
        PacketLossModel packetLossModel;
        if (strcmp(packetlossModelName.c_str(), "gilbertelliot") == 0) {
            packetLossModel = GILBERT_ELLIOT;
        } else if (strcmp(packetlossModelName.c_str(), "gilbert") == 0) {
            packetLossModel = GILBERT;
        } else if (strcmp(packetlossModelName.c_str(), "simplegilbert") == 0) {
            packetLossModel = SIMPLE_GILBERT;
        } else if (strcmp(packetlossModelName.c_str(), "bernouli") == 0) {
            packetLossModel = BERNOULI;
        } else if (strcmp(packetlossModelName.c_str(), "markov") == 0) {
            packetLossModel = MARKOV;
        }
        PacketLossToParameterParser packetLossToParameterParser(packetLossModel, filename);
        float *parameter = packetLossToParameterParser.parseParameter();
        if(packetLossModel != MARKOV) {
            cout << "p " << parameter[0] << endl;
            cout << "r " << parameter[1] << endl;
            cout << "k " << parameter[2] << endl;
            cout << "h " << parameter[3] << endl;
        } else {
            cout << "p13 " << parameter[0] << endl;
            cout << "p31 " << parameter[1] << endl;
            cout << "p32 " << parameter[2] << endl;
            cout << "p23 " << parameter[3] << endl;
            cout << "p14 " << parameter[4] << endl;
            cout << "p41 " << parameter[5] << endl;
        }
    } else {
        string modelname(argv[1]);
        std::transform(modelname.begin(), modelname.end(), modelname.begin(), ::tolower);

        PacketlossModel *model;
        unsigned int seed = static_cast<unsigned int>(atol(argv[2]));
        long numPackets = atol(argv[3]);

        if (strcmp(modelname.c_str(), "-real") == 0) {

        } else if (strcmp(modelname.c_str(), "markov") == 0) {
            if (argc != 9) {
                this->printModels();
            } else {
                float p13 = atof(argv[4]);
                float p31 = atof(argv[5]);
                float p32 = atof(argv[6]);
                float p23 = atof(argv[7]);
                float p14 = atof(argv[8]);
                float p41 = 1.0;
                model = new MarkovModel(seed, numPackets, p13, p31, p32, p23, p14, p41);
            }
        } else if (strcmp(modelname.c_str(), "gilbertelliot") == 0) {
            if (argc != 8) {
                this->printModels();
            } else {
                float p = atof(argv[4]);
                float r = atof(argv[5]);
                float k = atof(argv[6]);
                float h = atof(argv[7]);
                try{
                    model = new GilbertElliot(seed, numPackets, p, r, k, h);
                } catch (const exception& e){
                    this->printModels();
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
                try{
                    model = new GilbertElliot(seed, numPackets, p, r, k, h);
                } catch (const exception& e){
                    this->printModels();
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
                try{
                    model = new GilbertElliot(seed, numPackets, p, r, k, h);
                } catch (const exception& e){
                    this->printModels();
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
                try{
                    model = new GilbertElliot(seed, numPackets, p, r, k, h);
                } catch (const exception& e){
                    this->printModels();
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
    cout << "\tTraceGenerator [model] [args...]\tgenerates a trace with Model [model] and arguments [args]\n"
         << "\tTraceGenerator -showmodel\tshows all Models\n"
         << "\tTraceGenerator -real [trace-file]\n"
         << "\tTraceGenerator -extract [filename] [modelname]"
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
         << "]> <number_of_packets [1-" << numeric_limits<long>::max() << "]> <p13> <p31> <p32> <p23> <p14>" << endl;
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


