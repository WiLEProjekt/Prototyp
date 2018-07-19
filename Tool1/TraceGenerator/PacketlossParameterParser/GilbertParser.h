//
// Created by dennis on 19.07.18.
//

#ifndef TRACEGENERATOR_GILBERTPARSER_H
#define TRACEGENERATOR_GILBERTPARSER_H

#include <vector>
#include "BruteForceParser.h"

using namespace std;

class GilbertParser : BruteForceParser{
public:

    /**
     * Parses a 01-trace to parameter for a Gilbert model
     * @param trace the 01-trace
     * @return the parameter
     */
    float *parseParameter(vector<bool> trace);

private:
    /**
     * Useses the estimate-method to parse the parameter from a 01-trace
     * @param trace the 01-trace
     * @return the parameter
     */
    float *estimateParameter(vector<bool> trace);

    /**
     * Uses the brute-force-method to parse the parameter from a 01-trace
     * @param trace the 01-trace
     * @return the parameter
     */
    float *bruteForceParameter(vector<bool> trace);

};


#endif //TRACEGENERATOR_GILBERTPARSER_H
