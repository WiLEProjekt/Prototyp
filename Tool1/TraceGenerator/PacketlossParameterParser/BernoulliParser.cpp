//
// Created by dennis on 19.07.18.
//

#include "BernoulliParser.h"

float *BernoulliParser::parseParameter(vector<bool> trace) {
    unsigned long lossCounter = 0;
    for (auto &&i : trace) {
        if (!i) {
            lossCounter++;
        }
    }

    float p = 1.f / (float) trace.size() * (float) lossCounter;

    return new float[4]{p, 1 - p, 1, 0};
}
