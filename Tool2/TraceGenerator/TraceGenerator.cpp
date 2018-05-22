//
// Created by woors on 22.05.2018.
//

#include <cstring>
#include <algorithm>
#include "TraceGenerator.h"
#include "PaketlossModel/PaketlossModelType.h"
#include "PaketlossModel/MarkovModel.h"

TraceGenerator::TraceGenerator(int argc, char** argv) {
    if(argc < 2){
        this->printError();
    } if(argc == 2 && strcmp(argv[1], "-showmodel") == 0){
        this->printModels();
    } else {
        auto *args = new char[argc - 2];
        string modelname;
        this->parseArguments(argc, argv, &modelname, &args);
        std::transform(modelname.begin(), modelname.end(), modelname.begin(), ::tolower);

        PacketlossModel *model;

        if(strcmp(modelname, "markov") == 0){
            model = new MarkovModel();
        } else if(strcmp(modelname, "bernoulli") == 0){

        }
    }
}

void TraceGenerator::printError() {
    cout << "Korrekte Aufrufe:\n\tTraceGenerator [model] [args...]\tgeneriert einen Trace mit dem Model [modell] und "
            "den Argumenten [args]\n\tTraceGenerator -showmodel\tzeigt alle verfügbaren Modelle an" << endl;
}

void TraceGenerator::parseArguments(int argc, char** argv, string* model, char** args) {
    *model = argv[1];
    for(int i = 0; i < argc - 2; i++){
        args[i] = argv[i + 2];
    }
}

void TraceGenerator::printModels() {
    cout << "Verfügbare Modelle:\n Bernoulli\n Markov" << endl;
}


