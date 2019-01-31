
#include "TraceGenerator.h"

ExtractParameter
TraceGenerator::extractModelParameter(PacketLossModelType packetLossModel, vector<bool> parsedFile,
                                      unsigned int gMin = 0) {
    PacketLossToParameterParser packetLossToParameterParser(packetLossModel, parsedFile);

    ExtractParameter extractParameter{};
    if (gMin != 0) {
        extractParameter = packetLossToParameterParser.parseParameter(gMin);
    } else {
        extractParameter = packetLossToParameterParser.parseParameter();
    }
    double *parameter = extractParameter.parameter;
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

PacketLossModelType TraceGenerator::getPacketLossModelFromString(string modelname) {
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
         << "\tTraceGenerator -extract ping [filename] [modelname] [packetCount] ([gMin])"
         << "\textracts parameter of ping-file [filename] for the model [modelname]\n"
         << "\tTraceGenerator -import [icmp/tcp] [filename] [modelname] [outputfile] ([gMin]) ([seed])"
         << "\textracts parameter of tcp or icmp pcap-file [filename] and generates a new trace with [model] in [outputfile]\n"
         << "\tTraceGenerator -import ping [packetCount] [filename] [modelname] [outputfile] ([gMin]) ([seed])"
         << "\textracts parameter of trace-file [filename] for model [modelname] and generates a new trace in [outputfile]\n"
         << "\tTraceGenerator -parse [args]" << endl;
}

void TraceGenerator::printParseArgs() {
    cout
            << "TraceGenerator -parse -ping [filename of pingtrace] [total number of packets in the pingtrace] [output filename]\n"
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
    if (strcmp(proto.c_str(), "icmp") == 0) {
        return ICMP;
    } else if (strcmp(proto.c_str(), "tcp") == 0) {
        return TCP;
    } else {
        cout << proto << " is not a valid protocol" << endl;
        return NONE;
    }
}