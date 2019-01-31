#ifndef TRACEGENERATOR_GILBERTPARSER_H
#define TRACEGENERATOR_GILBERTPARSER_H

#include <vector>
#include "BruteForceParser.h"

using namespace std;

class GilbertParser : BruteForceParser {
public:

    /**
     * Parses a 01-trace to parameter for a Gilbert model
     * @param trace the 01-trace
     * @return the parameter
     */
    double *parseParameter(vector<bool> trace);

private:

    /**
     * Uses the brute-force-method to parse the parameter from a 01-trace
     * @param trace the 01-trace
     * @return the parameter
     */
    double *bruteForceParameter(vector<bool> trace);

};


#endif //TRACEGENERATOR_GILBERTPARSER_H
