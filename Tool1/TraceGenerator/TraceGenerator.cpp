
#include <cstring>
#include <algorithm>
#include "TraceGenerator.h"
#include "PaketlossModel/MarkovModel.h"
#include "PaketlossModel/GilbertElliot.h"
#include "TraceSaver.h"
#include "PacketLossToParameterParser.h"
#include "Pingparser.h"
#include <random>

ExtractParameter
TraceGenerator::extractModelParameter(const string &filename, string &packetlossModelName, unsigned int gMin) {
    std::transform(packetlossModelName.begin(), packetlossModelName.end(), packetlossModelName.begin(), ::tolower);
    PacketLossModelType packetLossModel = this->getPacketLossModelFromString(packetlossModelName);
    PacketLossToParameterParser packetLossToParameterParser(packetLossModel, filename);

    ExtractParameter extractParameter{};
    if (gMin != 0) {
        extractParameter = packetLossToParameterParser.parseParameter(gMin);
    } else {
        extractParameter = packetLossToParameterParser.parseParameter();
    }
    float * parameter = extractParameter.parameter;
    if (packetLossModel == MARKOV) {
        cout << "p13 " << parameter[0] << endl;
        cout << "p31 " << parameter[1] << endl;
        cout << "p32 " << parameter[2] << endl;
        cout << "p23 " << parameter[3] << endl;
        cout << "p14 " << parameter[4] << endl;
        cout << "p41 " << parameter[5] << endl;
    } else {
        cout << "p " << parameter[0] << endl;
        cout << "r " << parameter[1] << endl;
        cout << "k " << parameter[2] << endl;
        cout << "h " << parameter[3] << endl;
    }
    return extractParameter;
}

TraceGenerator::TraceGenerator(int argc, char **argv) {
    if (argc < 2) {
        this->printError();
        return;
    }
    if (strcmp(argv[1], "-showmodel") == 0) {
        this->printModels();
        return;
    } else if (strcmp(argv[1], "-extract") == 0) {
        string filename = argv[2];
        string packetlossModelName = argv[3];
        ExtractParameter parameter{};
        if (argc > 4) {
            unsigned int gMin = atoi(argv[4]);
            parameter = this->extractModelParameter(filename, packetlossModelName, gMin);
        } else {
            parameter = this->extractModelParameter(filename, packetlossModelName, 0);
        }
        delete[] (parameter.parameter);
    } else if (strcmp(argv[1], "-import") == 0) {
        string filename = argv[2];
        string packetlossModelName = argv[3];
        string outputFile = argv[4];
        PacketLossModelType packetLossModel = this->getPacketLossModelFromString(packetlossModelName);
        ExtractParameter extractParameter = this->extractModelParameter(filename, packetlossModelName, 0);


        BasePacketlossModel *model;
        if(packetLossModel == MARKOV) {
            model = new MarkovModel(extractParameter.packetCount, extractParameter.parameter);
        } else {
            model = new GilbertElliot(extractParameter.packetCount, extractParameter.parameter);
        }
        delete[] (extractParameter.parameter);
        vector<bool> trace = model->buildTrace();
        this->printPacketloss(trace);
        TraceSaver::writeTraceToFile(trace, outputFile);
    } else if (strcmp(argv[1], "-parse") == 0) {
        if (strcmp(argv[2], "-ping") == 0) {
            Pingparser().readPingFile(argv[3], atol(argv[4]));
        } else if (strcmp(argv[2], "-pcap") == 0) {
            Pingparser().readPcapFile(argv[3]);
        } else {
            this->printPingArgs();
        }
    } else if (strcmp(argv[1], "-gen") == 0) {
        if (argc < 4) {
            this->printModels();
            return;
        }
        string outputFile(argv[2]);
        string modelname(argv[3]);
        std::transform(modelname.begin(), modelname.end(), modelname.begin(), ::tolower);

        BasePacketlossModel *model = nullptr;
        unsigned int seed = static_cast<unsigned int>(atol(argv[4]));
        long numPackets = atol(argv[5]);

        if (strcmp(modelname.c_str(), "real") == 0) {

        } else if (strcmp(modelname.c_str(), "markov") == 0) {
            if (argc != 11) {
                this->printModels();
                return;
            } else {
                float p13 = atof(argv[6]);
                float p31 = atof(argv[7]);
                float p32 = atof(argv[8]);
                float p23 = atof(argv[9]);
                float p14 = atof(argv[10]);
                float p41 = 1.0;
                model = new MarkovModel(seed, numPackets, p13, p31, p32, p23, p14, p41);
            }
        } else if (strcmp(modelname.c_str(), "gilbertelliot") == 0) {
            if (argc != 10) {
                this->printModels();
                return;
            } else {
                float p = atof(argv[6]);
                float r = atof(argv[7]);
                float k = atof(argv[8]);
                float h = atof(argv[9]);
                try{
                    model = new GilbertElliot(seed, numPackets, p, r, k, h);
                } catch (const exception& e){
                    this->printModels();
                    return;
                }
            }

        } else if (strcmp(modelname.c_str(), "gilbert") == 0) {
            if (argc != 9) {
                this->printModels();
                return;
            } else {
                float p = atof(argv[6]);
                float r = atof(argv[7]);
                float k = 1;
                float h = atof(argv[8]);
                try{
                    model = new GilbertElliot(seed, numPackets, p, r, k, h);
                } catch (const exception& e){
                    this->printModels();
                    return;
                }
            }

        } else if (strcmp(modelname.c_str(), "simplegilbert") == 0) {
            if (argc != 8) {
                this->printModels();
                return;
            } else {
                float p = atof(argv[6]);
                float r = atof(argv[7]);
                float k = 1;
                float h = 0;
                try{
                    model = new GilbertElliot(seed, numPackets, p, r, k, h);
                } catch (const exception& e){
                    this->printModels();
                    return;
                }
            }

        } else if (strcmp(modelname.c_str(), "bernoulli") == 0) {
            if (argc != 7) {
                this->printModels();
                return;
            } else {
                float p = atof(argv[6]);
                float r = 1 - p;
                float k = 1;
                float h = 0;
                try{
                    model = new GilbertElliot(seed, numPackets, p, r, k, h);
                } catch (const exception& e){
                    this->printModels();
                    return;
                }
            }

        } else {
            cout << "No valid model: " << modelname << endl;
            this->printModels();
            return;
        }

        if (model != nullptr) {
            vector<bool> trace = model->buildTrace();
            delete (model);
            this->printPacketloss(trace);
            TraceSaver::writeTraceToFile(trace, outputFile);
        } else {
            printModels();
        }
    } else {
        printError();
    }
}

PacketLossModelType TraceGenerator::getPacketLossModelFromString(string modelname){
    std::transform(modelname.begin(), modelname.end(), modelname.begin(), ::tolower);
    PacketLossModelType packetLossModel;
    if (strcmp(modelname.c_str(), "gilbertelliot") == 0) {
        packetLossModel = GILBERT_ELLIOT;
    } else if (strcmp(modelname.c_str(), "gilbert") == 0) {
        packetLossModel = GILBERT;
    } else if (strcmp(modelname.c_str(), "simplegilbert") == 0) {
        packetLossModel = SIMPLE_GILBERT;
    } else if (strcmp(modelname.c_str(), "bernouli") == 0) {
        packetLossModel = BERNOULI;
    } else if (strcmp(modelname.c_str(), "markov") == 0) {
        packetLossModel = MARKOV;
    }
    return packetLossModel;
}

void TraceGenerator::printError() {
    cout << "\tTraceGenerator -gen [outputfile] [model] [args...]"
         << "\tgenerates a trace with Model [model] and arguments [args] in file [outputfile]\n"
         << "\tTraceGenerator -showmodel\tshows all Models\n"
         << "\tTraceGenerator -extract [filename] [modelname] ([gMin])"
         << "\textracts parameter of trace-file [filename] for the model [modelname]\n"
         << "\tTraceGenerator -import [filename] [modelname] [outputfile]"
         << "\textracts parameter of trace-file [filename] for model [modelname] and generates a new trace in [outputfile]\n"
         << "\tTraceGenerator -parse [args]" << endl;
}

void TraceGenerator::printPingArgs() {
    cout << "Pingparser -ping [filename of pingtrace] [output filename] [total number of packets in the pingrace]\n"
         << "Pingparser -pcap [filename of pcap trace .pcap] [output filename]" << endl;
}

void TraceGenerator::printModels() {
    cout << "Models:\n"
         << "\treal [filename]\n"
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
