//
// Created by dennis on 19.07.18.
//

#ifndef TRACEGENERATOR_SIMPLEGILBERTPARSER_H
#define TRACEGENERATOR_SIMPLEGILBERTPARSER_H

#include <vector>

using namespace std;

class SimpleGilbertParser {

public:
    /**
     * Parses a 01-trace to parameter for a Simple Gilbert model
     * @param trace the 01-trace
     * @return the paramter
     */
    float *parseParameter(vector<bool> trace);
};


#endif //TRACEGENERATOR_SIMPLEGILBERTPARSER_H
