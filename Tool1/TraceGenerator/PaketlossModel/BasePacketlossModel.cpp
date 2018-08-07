
#include <stdexcept>
#include <vector>
#include "BasePacketlossModel.h"
#include <iostream>
float BasePacketlossModel::generateRandomNumber() {
    return(distribution(generator));
}

void BasePacketlossModel::calculateBursts(vector<bool> trace, int i, int &losscounter,
                                          int &receivecounter, int &temp, vector<int> &genSizes) {
    //calculate burstsizes
    if(i > 1){
        if(!trace[i-1] && !trace[i-2]){
            losscounter++;
            temp--;
        }else if(trace[i-1] && trace[i-2]){
            receivecounter++;
            temp++;
        }else{
            genSizes.push_back(temp);
            temp = 0;
            if(!trace[i-1]){
                losscounter++;
                temp--;
            }
            else if(trace[i-1]) {
                receivecounter++;
                temp++;
            }
        }
    }else if(i==1){
        if(!trace[i-1]){
            losscounter++;
            temp--;
        }
        else if(trace[i-1]){
            receivecounter++;
            temp++;
        }
    }
    //return genSizes;



}
