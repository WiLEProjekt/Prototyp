
#ifndef TRACEGENERATOR_TRACEGENERATOR_H
#define TRACEGENERATOR_TRACEGENERATOR_H

#include <iostream>
#include <vector>

using namespace std;
class TraceGenerator {
private:
    /**
     * Prints an Error on the console, if the arguments aren't correct
     */
    void printError();
    /**
     * Prints the available models on the console
     */
    void printModels();

    void printModels(const exception& e);
    /**
     * Prints the packetloss in percent on the console
     * @param trace the loss trace
     */
    void printPacketloss(vector<bool> trace);
public:
    /**
     * Constructor
     * @param argc argument count
     * @param argv arguments
     */
    TraceGenerator(int argc, char** argv);
};


#endif //TRACEGENERATOR_TRACEGENERATOR_H
