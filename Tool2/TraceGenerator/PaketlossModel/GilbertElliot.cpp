//
// Created by drieke on 25.05.18.
//

#include <iostream>
#include "GilbertElliot.h"

vector<bool> GilbertElliot::buildTrace() {
    vector<bool> trace;
    bool good = true; // 1 = good state, 0 = bad state
    bool send = true; // 0 = loss, 1 = successfully send, or no loss
    for(int i = 0; i<numPackets; i++){
        if(good){ //in good state
            float randomNumberLossK = generateRandomNumber(); //Random Number that indicates if a packet is lost while being in a state
            float randomNumberStateP = generateRandomNumber(); //Random number that indicates a state transition from good -> bad
            send = !(randomNumberLossK <= (1.0-this->k)); //Calculate next state
            good = !(randomNumberStateP <= this->p); //Calculate next state
            trace.push_back(send);
        }else{ //in bad state
            float randomNumberLossH = generateRandomNumber(); //Random Number that indicates if a packet is lost while being in a state
            float randomNumberStateR = generateRandomNumber(); //Random number that indicates a state transition from bad -> good
            send = !(randomNumberLossH <= (1.0-this->h));
            good = (randomNumberStateR <= this->r); //Calculate next state
            trace.push_back(send);
        }
    }
    return trace;
}
