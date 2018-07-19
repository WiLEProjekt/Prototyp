//
// Created by dennis on 18.07.18.
//

#ifndef TRACEGENERATOR_GILBERTELLIOTPARSER_H
#define TRACEGENERATOR_GILBERTELLIOTPARSER_H

#include <vector>

using namespace std;

class GilbertElliotParser {
public:

    /**
     * Parses a 01-trace to parameter for a Gilbert-Elliot model
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


#endif //TRACEGENERATOR_GILBERTELLIOTPARSER_H
