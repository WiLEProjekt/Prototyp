#include "BernoulliParser.h"

double *BernoulliParser::parseParameter(vector<bool> trace) {
    unsigned long lossCounter = 0;
    for (auto &&i : trace) {
        if (!i) {
            lossCounter++;
        }
    }

    double p = 1.f / (double) trace.size() * (double) lossCounter;

    return new double[4]{p, 1 - p, 1, 0};
}
