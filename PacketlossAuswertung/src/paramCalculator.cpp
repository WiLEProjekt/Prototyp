#include <paramCalculator.h>

void getBernoulliParams(vector<bool> &trace, double &p){
    unsigned long lossCounter = 0;
    for (auto &&i : trace) {
        if (!i) {
            lossCounter++;
        }
    }

    p = 1.f / (double) trace.size() * (double) lossCounter;
}

void getSimpleGilbertParams(vector<bool> &trace, double &p, double &r){
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

    p = 1.f / (double) recieveCounter * (double) lossAfterRecieveCounter;
    r = 1.f / (double) lossCounter * (double) recieveAfterLossCounter;
}