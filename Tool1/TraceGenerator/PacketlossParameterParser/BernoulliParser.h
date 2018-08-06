#ifndef TRACEGENERATOR_BERNOULIPARSER_H
#define TRACEGENERATOR_BERNOULIPARSER_H

#include <vector>

using namespace std;

class BernoulliParser {

public:

    /**
     * Parses a 01-trace to parameter for a Bernoulli model
     * @param trace the 01-trace
     * @return the parameter
     */

    float *parseParameter(vector<bool> trace);
};


#endif //TRACEGENERATOR_BERNOULIPARSER_H
