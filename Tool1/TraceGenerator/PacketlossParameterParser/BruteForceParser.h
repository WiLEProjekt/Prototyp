//
// Created by dennis on 19.07.18.
//

#ifndef TRACEGENERATOR_BRUTEFORCEPARSER_H
#define TRACEGENERATOR_BRUTEFORCEPARSER_H

#include <vector>
#include <iostream>

using namespace std;

class BruteForceParser {
protected:
    void calcLoss(vector<bool> &trace, float &result, float &burstsize, float &goodsize);

    virtual vector<float>
    checkResult(vector<vector<float>> avgburstsizes, float origburstsize, vector<vector<float>> possibleParams,
                float origLoss, float origgoodsize);
};


#endif //TRACEGENERATOR_BRUTEFORCEPARSER_H
