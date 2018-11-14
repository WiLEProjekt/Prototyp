#ifndef TRACEGENERATOR_BRUTEFORCEPARSER_H
#define TRACEGENERATOR_BRUTEFORCEPARSER_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

class BruteForceParser {
protected:
    void calcLoss(vector<bool> &trace, float &lossrate, float &burstsize, float &goodsize, vector<int> &overallsizes);

    void calcDistFunction(vector<int> &sizes, vector<vector<float> > &distFunction);

    bool kstest(vector<vector<float> > origDistFunction, vector<vector<float> > &generatedDistFunction, long m, long n);

    void findTopX(vector<vector<float> > &top, vector<vector<float> > &possibleParams, int x, string model);

    virtual vector<float> checkResult(vector<vector<float>> avgburstsizes, float origburstsize, vector<vector<float>> possibleParams,
                float origLoss, float origgoodsize);
};


#endif //TRACEGENERATOR_BRUTEFORCEPARSER_H
