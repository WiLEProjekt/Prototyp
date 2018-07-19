
#include <stdexcept>
#include <vector>
#include "BasePacketlossModel.h"
#include <iostream>
float BasePacketlossModel::generateRandomNumber() {
    return(distribution(generator));
}

void BasePacketlossModel::calculateBursts(vector<bool> trace, unsigned long i, unsigned long &losscounter,
                                          unsigned long &receivecounter, long &temp,
                                          vector<unsigned long> &burstsizes, vector<unsigned long> &goodsizes) {
    if (i > 1) {
        if (!trace[i - 1] && !trace[i - 2]) {
            losscounter++;
            temp--;
        } else if (trace[i - 1] && trace[i - 2]) {
            receivecounter++;
            temp++;
        } else {
            if (temp < 0) {
                burstsizes.push_back(temp * (-1));
            } else {
                goodsizes.push_back(temp);
            }
            temp = 0;
            if (!trace[i - 1]) {
                losscounter++;
                temp--;
            } else if (trace[i - 1]) {
                receivecounter++;
                temp++;
            }
        }
    } else if (i == 1) {
        if (!trace[i - 1]) {
            losscounter++;
            temp--;
        } else if (trace[i - 1]) {
            receivecounter++;
            temp++;
        }
    }

}
