
#ifndef TRACEGENERATOR_TRACEGENERATOR_H
#define TRACEGENERATOR_TRACEGENERATOR_H

#include <iostream>
#include <vector>
#include <stdlib.h>
#include "Packetloss/PacketlossParameterParser/PacketLossToParameterParser.h"
#include "Packetloss/PaketlossModel/MarkovModel.h"
#include "Packetloss/PaketlossModel/GilbertElliotModel.h"
#include "Packetloss/PacketlossParameterParser/PacketLossToParameterParser.h"
#include "Pingparser.h"
#include "TraceSaver.h"
#include "Pingparser.h"
#include <random>
#include <cstring>
#include <algorithm>


using namespace std;
class TraceGenerator {
private:

    /**
     * Extracts Modelparameters
     *
     * @param fileType the type of the file "tcp" or "icmp" for .pcap or "ping" for a ping-logfile
     * @param filename the Filename of the trace
     * @param packetlossModelName the name of the model
     * @param gMin gMin
     * @param packetCount number of packets. Only required if fileType is "ping"
     */
    void startExtract(int argc, char **argv);

    void startImport(int argc, char **argv);

    void startParse(int argc, char **argv);


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
    ExtractParameter
    extractModelParameter(const string &filename, string &fileType, string &packetlossModelName, unsigned int gMin);

    ExtractParameter
    extractModelParameter(PacketLossModelType packetLossModel, vector<bool> parsedFile, unsigned int gMin);

    ExtractParameter
    extractModelParameterFromPing(const string &filename, unsigned int packetCount, string &packetlossModelName);
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

    void startGen(int argc, char **argv);
};


#endif //TRACEGENERATOR_TRACEGENERATOR_H
