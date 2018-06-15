

#include <iostream>
#include "GilbertElliot.h"

vector<bool> GilbertElliot::buildTrace() {
    vector<bool> trace;
    bool good = true; // 1 = good state, 0 = bad state
    bool send = true; // 0 = loss, 1 = successfully send, or no loss
    for(long i = 0; i< this->numPackets+1; i++){
        if(good){ //in good state
            float randomNumberLossK = generateRandomNumber(); //Random Number that indicates if a packet is lost while being in a state
            float randomNumberStateP = generateRandomNumber(); //Random number that indicates a state transition from good -> bad
            send = !(randomNumberLossK <= (1.0-this->k)); //Calculate next state
            good = !(randomNumberStateP <= this->p); //Calculate next state
           // cout << "random k: " << randomNumberLossK << " k: " << this->k << " random p: " << randomNumberStateP << " p: " << this->p << endl;

        }else{ //in bad state
            float randomNumberLossH = generateRandomNumber(); //Random Number that indicates if a packet is lost while being in a state
            float randomNumberStateR = generateRandomNumber(); //Random number that indicates a state transition from bad -> good
            send = !(randomNumberLossH <= (1.0-this->h));
            good = (randomNumberStateR <= this->r); //Calculate next state
            //cout << "random h: " << randomNumberLossH << " h: " << this->h << " random r: " << randomNumberStateR << " r: " << this->r << endl;
        }
        if(i != 0) {
            trace.push_back(send);
        }
    }
    return trace;
}

string GilbertElliot::checkParameter() {
    if (numPackets < 1 || numPackets > numeric_limits<long>::max() - 1) {
        return "numPackets";
    } else if (p < 0 || p > 1) {
        return "p";
    } else if (r < 0 || r > 1) {
        return "r";
    } else if (k < 0 || k > 1) {
        return "k";
    } else if (h < 0 || h > 1) {
        return "h";
    }
    return nullptr;
}
