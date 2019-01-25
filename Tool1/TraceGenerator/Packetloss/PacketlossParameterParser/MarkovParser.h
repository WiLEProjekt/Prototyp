#ifndef TRACEGENERATOR_MARKOVPARSER_H
#define TRACEGENERATOR_MARKOVPARSER_H

#include <vector>
#include <map>
#include "BruteForceParser.h"

using namespace std;

class MarkovParser : BruteForceParser{
public:

    /**
     * Parses a 01-trace to parameter for a 4-state-markov model
     * @param trace the 01-trace
     * @param gMin the burst-size
     * @return the parameter
     */
    float *parseParameter(vector<bool> trace, unsigned int gMin);

private:
    /**
     * Useses the estimate-method to parse the parameter from a 01-trace
     * @param trace the 01-trace
     * @return the parameter
     */
    float *estimateParameter(vector<bool> trace, unsigned int gMin);

    /**
     * Uses the brute-force-method to parse the parameter from a 01-trace
     * @param trace the 01-trace
     * @return the parameter
     */
    float *bruteForceParameter(vector<bool> trace);
};


#endif //TRACEGENERATOR_MARKOVPARSER_H
