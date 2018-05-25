
#include <iostream>
#include "MarkovModel.h"

vector<bool> MarkovModel::buildTrace() {
    vector<bool> trace;
    bool gap = false; //Gap period. 0 = packet received during gap period (state 1), 1 = packet lost during gap period (state 4)
    bool burst = false; //Burst period. 0 = packet received during burst period (state 2), 1 = packet lost during burst period (state 3)

    for (long i = 0; i < numPackets+1; i++) {
        //TODO: Implementiere 4 Zustand markov Modell hier
    }
    return trace;
}

