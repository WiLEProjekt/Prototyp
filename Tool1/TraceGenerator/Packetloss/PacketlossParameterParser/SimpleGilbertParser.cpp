#include "SimpleGilbertParser.h"

double *SimpleGilbertParser::parseParameter(vector<bool> trace) {
    unsigned long lossCounter = 0;
    unsigned long recieveCounter = 0;
    unsigned long lossAfterRecieveCounter = 0;
    unsigned long recieveAfterLossCounter = 0;

    for (int i = 0; i < trace.size(); i++) {
        if (trace[i]) {
            recieveCounter++;
            if (i != 0) {
                if (!trace[i - 1]) {
                    recieveAfterLossCounter++;
                }
            }
        } else {
            lossCounter++;
            if (i != 0) {
                if (trace[i - 1]) {
                    lossAfterRecieveCounter++;
                }
            }
        }
    }

    double p = 1.f / (double) recieveCounter * (double) lossAfterRecieveCounter;
    double r = 1.f / (double) lossCounter * (double) recieveAfterLossCounter;

    return new double[4]{p, r, 1, 0};
}
