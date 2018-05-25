//
// Created by woors on 22.05.2018.
//

#include <cstring>
#include <algorithm>
#include "TraceGenerator.h"
#include "PaketlossModel/PaketlossModelType.h"
#include "PaketlossModel/MarkovModel.h"
#include "PaketlossModel/GilbertElliot.h"
#include "TraceSaver.h"

TraceGenerator::TraceGenerator(int argc, char** argv) {
    if(argc < 2){
        this->printError();
    } if(argc == 2 && strcmp(argv[1], "-showmodel") == 0){
        this->printModels();
    } else {
        string modelname(argv[1]);
        std::transform(modelname.begin(), modelname.end(), modelname.begin(), ::tolower);

        PacketlossModel *model;

        if (strcmp(modelname.c_str(), "markov") == 0) {
            model = new MarkovModel(atoi(argv[2]));
        } else if (strcmp(modelname.c_str(), "gilbertelliot") == 0) {
            int numPackets = atoi(argv[3]);
            float p = atof(argv[4]);
            float r = atof(argv[5]);
            float k = atof(argv[6]);
            float h = atof(argv[7]);
            model = new GilbertElliot(atoi(argv[2]),numPackets, p, r, k, h);
        } else if (strcmp(modelname.c_str(), "gilbert") == 0) {
            int numPackets = atoi(argv[3]);
            float p = atof(argv[4]);
            float r = atof(argv[5]);
            float k = 1;
            float h = atof(argv[6]);
            model = new GilbertElliot(atoi(argv[2]),numPackets, p, r, k, h);
        }else if (strcmp(modelname.c_str(), "simplegilbert") == 0) {
            int numPackets = atoi(argv[3]);
            float p = atof(argv[4]);
            float r = atof(argv[5]);
            float k = 1;
            float h = 0;
            model = new GilbertElliot(atoi(argv[2]),numPackets, p, r, k, h);
        }else if (strcmp(modelname.c_str(), "bernoulli") == 0) {
            int numPackets = atoi(argv[3]);
            float p = atof(argv[4]);
            float r = 1-p;
            float k = 1;
            float h = 0;
            model = new GilbertElliot(atoi(argv[2]),numPackets, p, r, k, h);
        }else {
            cout << "Kein Gültiges Modell gewählt" << endl;
            this->printModels();
            return;
        }

        vector<bool> trace = model->buildTrace();
        this->printPacketloss(trace);
        TraceSaver::writeTraceToFile(trace);
    }
}

void TraceGenerator::printError() {
    cout << "Korrekte Aufrufe:\n\tTraceGenerator [model] [args...]\tgeneriert einen Trace mit dem Model [modell] und "
            "den Argumenten [args]\n\tTraceGenerator -showmodel\tzeigt alle verfügbaren Modelle an" << endl;
}

void TraceGenerator::printModels() {
    cout << "Verfügbare Modelle:\n GilbertElliot [seed] [p] [r] [k] [h]\nGilbert [seed] [p] [r] [h]\n"
            "SimpleGilbert [seed] [p] [r]\nBernoulli [seed] [p]\n Markov [seed]" << endl;
}

void TraceGenerator::printPacketloss(vector<bool> trace){
    long zeros = 0;
    for (auto &&i : trace) {
        if(i == false){
            zeros++;
        }
    }
    cout << " Packetloss: " << (100/(float)trace.size()) * (float)zeros << "%" << endl;
}


