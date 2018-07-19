//
// Created by dennis on 19.07.18.
//

#include <cmath>
#include "BruteForceParser.h"


void BruteForceParser::calcLoss(vector<bool> &trace, float &result, float &burstsize, float &goodsize) {
    vector<int> burstsizes;
    vector<int> goodsizes;
    int temp = 0;
    int losscounter = 0;
    int receivecounter = 0;
    for (int i = 0; i < trace.size(); i++) {
        if (i > 0) {
            if (!trace[i] && !trace[i - 1]) {
                losscounter++;
                temp--;
            } else if (trace[i] && trace[i - 1]) {
                receivecounter++;
                temp++;
            } else {
                if (temp < 0) {
                    burstsizes.push_back((temp) * (-1));
                } else {
                    goodsizes.push_back(temp);
                }
                temp = 0;
                if (!trace[i]) {
                    losscounter++;
                    temp--;
                } else if (trace[i]) {
                    receivecounter++;
                    temp++;
                }
            }
        } else {
            if (!trace[i]) {
                losscounter++;
                temp--;
            } else if (trace[i]) {
                receivecounter++;
                temp++;
            }
        }
    }
    result = (float) losscounter / trace.size() * 100;
    burstsize = (float) losscounter / burstsizes.size();
    goodsize = (float) receivecounter / goodsizes.size();
}

vector<float> BruteForceParser::checkResult(vector<vector<float>> avgburstsizes, float origburstsize,
                                            vector<vector<float>> possibleParams, float origLoss, float origgoodsize) {
    float lowestdiff = 1000;
    int lowestindex = 0;
    for (int i = 0; i < avgburstsizes.size(); i++) {
        if ((fabs(avgburstsizes[i][1] - origburstsize) + fabs(avgburstsizes[i][2] - origgoodsize)) < lowestdiff) {
            lowestdiff = fabs(avgburstsizes[i][1] - origburstsize) + fabs(avgburstsizes[i][2] - origgoodsize);
            lowestindex = i;
        }
    }

    cout << "found parameters: p: " << possibleParams[lowestindex][0] << " r: " << possibleParams[lowestindex][1]
         << " h: " << possibleParams[lowestindex][2] << endl;
    cout << "original Lossrate: " << origLoss << " original avg Burstsize: " << origburstsize << " original good size: "
         << origgoodsize << endl;
    cout << "found Lossrate: " << avgburstsizes[lowestindex][0] << " found avg Burstsize: "
         << avgburstsizes[lowestindex][1] << endl;
    return possibleParams[lowestindex];
}

