
#include <iostream>
#include "MarkovModel.h"

vector<bool> MarkovModel::buildTrace() {
    vector<bool> trace;
    int state = 1;

    for (long i = 0; i < numPackets + 1; i++) {
        float randomValue = this->generateRandomNumber();
        bool value;
        switch (state) {
            case 1:
                if (randomValue <= this->p13) {
                    state = 3;
                } else if (randomValue <= this->p13 + this->p14) {
                    state = 4;
                }
                value = true;
                break;
            case 2:
                if (randomValue <= this->p23) {
                    state = 3;
                }
                value = true;
                break;
            case 3:
                if (randomValue <= this->p32) {
                    state = 2;
                } else if(randomValue <= this->p32 + this->p31){
                    state = 1;
                }
                value = false;
                break;
            case 4:
                if (randomValue <= this->p41) {
                    state = 1;
                }
                value = false;
                break;
        }
        if (i != 0) {
            trace.push_back(value);
        }
    }
    return trace;
}

vector<int> MarkovModel::buildTrace2() {
    vector<bool> trace;
    vector<int> gensizes;
    int temp = 0;
    int losscounter = 0;
    int receivecounter = 0;
    int state = 1;

    for (int i = 0; i < numPackets + 1; i++) {
        float randomValue = this->generateRandomNumber();
        bool value;
        switch (state) {
            case 1:
                if (randomValue <= this->p13) {
                    state = 3;
                } else if (randomValue <= this->p13 + this->p14) {
                    state = 4;
                }
                value = true;
                break;
            case 2:
                if (randomValue <= this->p23) {
                    state = 3;
                }
                value = true;
                break;
            case 3:
                if (randomValue <= this->p32) {
                    state = 2;
                } else if (randomValue <= this->p32 + this->p31) {
                    state = 1;
                }
                value = false;
                break;
            case 4:
                if (randomValue <= this->p41) {
                    state = 1;
                }
                value = false;
                break;
        }
        if (i != 0) {
            trace.push_back(value);
        }

        //calculate burstsizes
        calculateBursts(trace, i, losscounter, receivecounter, temp, gensizes);
    }
    return(gensizes);

}
