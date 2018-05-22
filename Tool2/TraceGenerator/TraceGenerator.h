//
// Created by woors on 22.05.2018.
//

#ifndef TRACEGENERATOR_TRACEGENERATOR_H
#define TRACEGENERATOR_TRACEGENERATOR_H

#include <iostream>

using namespace std;
class TraceGenerator {
private:
    void printError();
    void parseArguments(int argc, char** argv, string* model, char** args);
    void printModels();
public:
    TraceGenerator(int argc, char** argv);

};


#endif //TRACEGENERATOR_TRACEGENERATOR_H
