#ifndef TRACEGENERATOR_GILBERTELLIOTPARSER_H
#define TRACEGENERATOR_GILBERTELLIOTPARSER_H

#include <vector>
#include "BruteForceParser.h"

using namespace std;

class GilbertElliotParser : BruteForceParser {
public:

    /**
     * Parses a 01-trace to parameter for a Gilbert-Elliot model
     * @param trace the 01-trace
     * @param gMin the burst-size
     * @return the parameter
     */
    double *parseParameter(vector<bool> trace, unsigned int gMin);


private:

    /**
     * Uses the brute-force-method to parse the parameter from a 01-trace
     * @param trace the 01-trace
     * @return the parameter
     */
    double *bruteForceParameter(vector<bool> trace);
};


#endif //TRACEGENERATOR_GILBERTELLIOTPARSER_H
