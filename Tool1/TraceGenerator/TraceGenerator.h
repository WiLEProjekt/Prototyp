
#ifndef TRACEGENERATOR_TRACEGENERATOR_H
#define TRACEGENERATOR_TRACEGENERATOR_H

#include <iostream>
#include <vector>
#include "PacketLossToParameterParser.h"
#include "Pingparser.h"

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

    /**
     * Prints the packetloss in percent on the console
     * @param trace the loss trace
     */
    void printPacketloss(vector<bool> trace);

    /**
     * Prints the arguments for the parse option
     */
    void printParseArgs();

    /**
     * Returns the PacketLossModel with the name @param modelname
     * @param modelname the name of the model
     * @return the packetLossModel
     */
    PacketLossModelType getPacketLossModelFromString(string modelname);

    /**
     * Extracts the ModelParameter of a file with 01-trace
     * @param filename the file
     * @param packetlossModelName the Model for the paramter
     * @param gMin the burst size
     * @return the parameter for the model
     */
    ExtractParameter extractModelParameter(const string &filename, string &packetlossModelName, unsigned int gMin);

    /**
    * Returns the Protocol with the name @param proto
    * @param proto the name of the protocol
    * @return the Protocol
    */
    Protocol parseProtocol(string proto);
public:

    /**
     * Constructor
     * @param argc argument count
     * @param argv arguments
     */
    TraceGenerator(int argc, char** argv);
};


#endif //TRACEGENERATOR_TRACEGENERATOR_H
