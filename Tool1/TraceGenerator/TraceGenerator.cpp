
#include <cstring>
#include <algorithm>
#include "TraceGenerator.h"
#include "PaketlossModel/MarkovModel.h"
#include "PaketlossModel/GilbertElliotModel.h"
#include "TraceSaver.h"
#include "PacketlossParameterParser/PacketLossToParameterParser.h"
#include "Pingparser.h"
#include <random>

ExtractParameter TraceGenerator::extractModelParameterFromPing(const string &filename, unsigned int packetCount,
                                                               string &packetlossModelName) {
    std::transform(packetlossModelName.begin(), packetlossModelName.end(), packetlossModelName.begin(), ::tolower);
    PacketLossModelType packetLossModel = this->getPacketLossModelFromString(packetlossModelName);
    vector<bool> parsedFile = Pingparser().readPingFile(filename, packetCount);

    return this->extractModelParameter(packetLossModel, parsedFile, 0);
}

ExtractParameter
TraceGenerator::extractModelParameter(PacketLossModelType packetLossModel, vector<bool> parsedFile, unsigned int gMin = 0) {
    PacketLossToParameterParser packetLossToParameterParser(packetLossModel, parsedFile);

    ExtractParameter extractParameter{};
    if (gMin != 0) {
        extractParameter = packetLossToParameterParser.parseParameter(gMin);
    } else {
        extractParameter = packetLossToParameterParser.parseParameter();
    }
    float *parameter = extractParameter.parameter;
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

ExtractParameter
TraceGenerator::extractModelParameter(const string &filename, string &fileType, string &packetlossModelName, unsigned int gMin = 0) {
    std::transform(packetlossModelName.begin(), packetlossModelName.end(), packetlossModelName.begin(), ::tolower);
    std::transform(fileType.begin(), fileType.end(), fileType.begin(), ::tolower);
    PacketLossModelType packetLossModel = this->getPacketLossModelFromString(packetlossModelName);

    vector<bool> parsedFile;

    if (strcmp(fileType.c_str(), "icmp") == 0) {
        parsedFile = Pingparser().readPcapFile(filename, ICMP);
    } else if (strcmp(fileType.c_str(), "tcp") == 0) {
        parsedFile = Pingparser().readPcapFile(filename, TCP);
    }else{
        parsedFile = Pingparser().readFile(filename);
        //cout << parsedFile.size() << " " << parsedFile[parsedFile.size()-1] << endl;
    }

    return extractModelParameter(packetLossModel, parsedFile, gMin);
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
        if (argc < 5) {
            this->printError();
            return;
        }
        string fileType = argv[2];
        string filename = argv[3];
        string packetlossModelName = argv[4];
        ExtractParameter parameter{};
        if (strcmp(fileType.c_str(), "ping") == 0) {
            if (argc > 5) {
                filename = argv[4];
                packetlossModelName = argv[5];
                parameter = this->extractModelParameterFromPing(filename, atol(argv[3]), packetlossModelName);
            }
        } else {
            if (argc > 5) {
                unsigned int gMin = atoi(argv[5]);
                parameter = this->extractModelParameter(filename, fileType, packetlossModelName, gMin);
            } else {
                //cout << filename << " " << fileType << " " << packetlossModelName << endl;
                parameter = this->extractModelParameter(filename, fileType, packetlossModelName, 0);
            }
        }
        delete[] (parameter.parameter);
    } else if (strcmp(argv[1], "-import") == 0) {
        unsigned int gMin = 0;
        unsigned int seed = time(0);
        ExtractParameter extractParameter;
        if (argc < 6) {
            this->printError();
            return;
        }
        string fileType = argv[2];
        string filename = argv[3];
        string packetlossModelName = argv[4];
        string outputFile = argv[5];
        if (strcmp(fileType.c_str(), "ping") == 0) {
            if (argc > 5) {
                unsigned int packetCount = atol(argv[3]);
                filename = argv[4];
                packetlossModelName = argv[5];
                outputFile = argv[6];

                if (argc > 7) {
                    gMin = atoi(argv[7]);
                }
                if (argc > 8) {
                    seed = atol(argv[8]);
                }
                extractParameter = this->extractModelParameterFromPing(filename, packetCount, packetlossModelName);
            }
        } else {
            if (argc > 6) {
                gMin = atoi(argv[6]);
            }
            if (argc > 7) {
                seed = atol(argv[7]);
            }
            extractParameter = this->extractModelParameter(filename, fileType, packetlossModelName, gMin);
        }


        PacketLossModelType packetLossModel = this->getPacketLossModelFromString(packetlossModelName);
        BasePacketlossModel *model;
        if(packetLossModel == MARKOV) {
            model = new MarkovModel(extractParameter.packetCount, seed, extractParameter.parameter);
        } else {
            model = new GilbertElliotModel(extractParameter.packetCount, seed, extractParameter.parameter);
        }
        vector<bool> trace = model->buildTrace();
        delete[] (extractParameter.parameter);
        this->printPacketloss(trace);
        TraceSaver::writeTraceToFile(trace, outputFile);
        delete (model);
    } else if (strcmp(argv[1], "-parse") == 0) {
        if(argc < 3){
            this->printParseArgs();
            return ;
        }
        if (strcmp(argv[2], "-ping") == 0) {
            if(argc < 6){
                this->printParseArgs();
                return ;
            }
            Pingparser().readPingFile(argv[3], atol(argv[4]), argv[5]);

        } else if (strcmp(argv[2], "-pcap") == 0) {
            if(argc < 6){
                this->printParseArgs();
                return ;
            }
            string proto = argv[3];
            std::transform(proto.begin(), proto.end(), proto.begin(), ::tolower);
            Protocol protocol = this->parseProtocol(proto);
            if(protocol != NONE) {
                Pingparser().readPcapFile(argv[4], protocol, argv[5]);
            }
        } else {
            this->printParseArgs();
        }
    } else if (strcmp(argv[1], "-gen") == 0) {
        if (argc < 6) {
            this->printModels();
            return;
        }
        string outputFile(argv[2]);
        string modelname(argv[3]);
        std::transform(modelname.begin(), modelname.end(), modelname.begin(), ::tolower);

        BasePacketlossModel *model = nullptr;
        auto seed = static_cast<unsigned int>(atol(argv[4]));
        long numPackets = atol(argv[5]);

        if (strcmp(modelname.c_str(), "markov") == 0) {
            if (argc != 11) {
                this->printModels();
                return;
            } else {
                float p13 = atof(argv[6]);
                float p31 = atof(argv[7]);
                float p32 = atof(argv[8]);
                float p23 = atof(argv[9]);
                float p14 = atof(argv[10]);
                model = new MarkovModel(seed, numPackets, p13, p31, p32, p23, p14);
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
                    model = new GilbertElliotModel(seed, numPackets, p, r, k, h);
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
                    model = new GilbertElliotModel(seed, numPackets, p, r, k, h);
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
                    model = new GilbertElliotModel(seed, numPackets, p, r, k, h);
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
                    model = new GilbertElliotModel(seed, numPackets, p, r, k, h);
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
            this->printPacketloss(trace);
            TraceSaver::writeTraceToFile(trace, outputFile);
            delete (model);
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
    } else if (strcmp(modelname.c_str(), "bernoulli") == 0) {
        packetLossModel = BERNOULLI;
    } else if (strcmp(modelname.c_str(), "markov") == 0) {
        packetLossModel = MARKOV;
    } else {
        printError();
        throw invalid_argument("Model " + modelname + " is not defined");
    }
    return packetLossModel;
}

void TraceGenerator::printError() {
    cout << "\tTraceGenerator -gen [outputfile] [model] [args...]"
         << "\tgenerates a trace with Model [model] and arguments [args] in file [outputfile]\n"
         << "\tTraceGenerator -showmodel\tshows all Models\n"
         << "\tTraceGenerator -extract [tcp/icmp] [filename] [modelname] ([gMin])"
         << "\textracts parameter of a tcp or icmp pcap-file [filename] for the model [modelname]\n"
         << "\tTraceGenerator -extract ping [packetCount] [filename] [modelname] ([gMin])"
         << "\textracts parameter of ping-file [filename] for the model [modelname]\n"
         << "\tTraceGenerator -import [icmp/tcp] [filename] [modelname] [outputfile] ([gMin]) ([seed])"
         << "\textracts parameter of tcp or icmp pcap-file [filename] and generates a new trace with [model] in [outputfile]\n"
         << "\tTraceGenerator -import ping [packetCount] [filename] [modelname] [outputfile] ([gMin]) ([seed])"
         << "\textracts parameter of trace-file [filename] for model [modelname] and generates a new trace in [outputfile]\n"
         << "\tTraceGenerator -parse [args]" << endl;
}

void TraceGenerator::printParseArgs() {
    cout << "TraceGenerator -parse -ping [filename of pingtrace] [total number of packets in the pingtrace] [output filename]\n"
         << "TraceGenerator -parse -pcap [TCP|ICMP] [filename of pcap trace .pcap] [output filename]" << endl;
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

Protocol TraceGenerator::parseProtocol(string proto) {
    if(strcmp(proto.c_str(), "icmp") == 0){
        return ICMP;
    } else if (strcmp(proto.c_str(), "tcp") == 0){
        return TCP;
    } else {
        cout << proto << " is not a valid protocol" << endl;
        return NONE;
    }
}
