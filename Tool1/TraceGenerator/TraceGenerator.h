
#ifndef TRACEGENERATOR_TRACEGENERATOR_H
#define TRACEGENERATOR_TRACEGENERATOR_H

#include <iostream>
#include <vector>

using namespace std;
class TraceGenerator {
private:
    void printError();
    void printModels();
    void printPacketloss(vector<bool> trace);
public:
    TraceGenerator(int argc, char** argv);
};


#endif //TRACEGENERATOR_TRACEGENERATOR_H
