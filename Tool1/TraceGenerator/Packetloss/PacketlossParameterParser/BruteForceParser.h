#ifndef TRACEGENERATOR_BRUTEFORCEPARSER_H
#define TRACEGENERATOR_BRUTEFORCEPARSER_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

using namespace std;

class BruteForceParser {
protected:
    void
    calcLoss(vector<bool> &trace, double &lossrate, double &burstsize, double &goodsize, vector<int> &overallsizes);

    void calcDistFunction(vector<int> &sizes, vector<vector<double> > &distFunction);

    bool
    kstest(vector<vector<double> > origDistFunction, vector<vector<double> > &generatedDistFunction, long m, long n,
           double &dvalue);

    void findTopX(vector<vector<double> > &top, vector<vector<double> > &possibleParams, int x, string model);

    virtual vector<double>
    checkResult(vector<vector<double>> avgburstsizes, double origburstsize, vector<vector<double>> possibleParams,
                double origLoss, double origgoodsize);
};


#endif //TRACEGENERATOR_BRUTEFORCEPARSER_H
