#include <cmath>
#include "BruteForceParser.h"


void BruteForceParser::calcLoss(vector<bool> &trace, double &lossrate, double &burstsize, double &goodsize,
                                vector<int> &overallsizes) {
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
                    overallsizes.push_back(temp);
                } else {
                    goodsizes.push_back(temp);
                    overallsizes.push_back(temp);
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
    lossrate = (double) losscounter / trace.size() * 100;
    burstsize = (double) losscounter / burstsizes.size();
    goodsize = (double) receivecounter / goodsizes.size();
}

void BruteForceParser::calcDistFunction(vector<int> &sizes, vector<vector<double> > &distFunction) {
    double cumprob = 0;
    for (int i = sizes[0]; i <= sizes[sizes.size() - 1]; i++) {
        vector<double> temp;
        int counter = count(sizes.begin(), sizes.end(), i);
        double tmp = (double) i;
        temp.push_back(tmp);
        double prob = cumprob + (double) counter / sizes.size();
        temp.push_back(prob);
        cumprob = prob;
        distFunction.push_back(temp);
    }
}

//finds top x best matching parameters
void
BruteForceParser::findTopX(vector<vector<double> > &top, vector<vector<double> > &possibleParams, int x, string model) {
    for (int a = 0; a < x; a++) {
        int minindex = 0;
        for (int i = 0; i < possibleParams.size(); i++) {
            if (possibleParams[i][2] < possibleParams[minindex][2]) {
                minindex = i;
            }
        }
        vector<double> tmp;
        if (model.compare("Gilbert") == 0) {
            tmp.push_back(possibleParams[minindex][3]);
            tmp.push_back(possibleParams[minindex][4]);
            tmp.push_back(possibleParams[minindex][5]);
        } else if (model.compare("GilbertElliot") == 0) {
            tmp.push_back(possibleParams[minindex][3]);
            tmp.push_back(possibleParams[minindex][4]);
            tmp.push_back(possibleParams[minindex][5]);
            tmp.push_back(possibleParams[minindex][6]);
        } else if (model.compare("Markov") == 0) {
            tmp.push_back(possibleParams[minindex][3]);
            tmp.push_back(possibleParams[minindex][4]);
            tmp.push_back(possibleParams[minindex][5]);
            tmp.push_back(possibleParams[minindex][6]);
            tmp.push_back(possibleParams[minindex][7]);
            tmp.push_back(possibleParams[minindex][1]);
        }
        top.push_back(tmp);
        possibleParams.erase(possibleParams.begin() + minindex);
    }

}

//Two-sided Kolmogorov-Smirnov Test
bool BruteForceParser::kstest(vector<vector<double> > origDistFunction, vector<vector<double> > &generatedDistFunction,
                              long m, long n, double &dvalue) {
    double alpha = 0.01; //significanceniveau
    double decider = sqrt(-0.5 * log(alpha / 2) * (n + m) / (n * m));
    double d = 0; //ks test value

    //bring both distribution functions to the same size by adding elements at the beginning and at the end
    while (origDistFunction[0][0] > generatedDistFunction[0][0]) { //add elements at the beginning of origDistFunction
        vector<double> temp;
        double i = origDistFunction[0][0] - 1;
        temp.push_back(i);
        temp.push_back(0.0f);
        origDistFunction.insert(origDistFunction.begin(), temp);
    }
    while (generatedDistFunction[0][0] >
           origDistFunction[0][0]) { //add elements at the beginning of generatedDistFunction
        vector<double> temp;
        double i = generatedDistFunction[0][0] - 1;
        temp.push_back(i);
        temp.push_back(0.0f);
        generatedDistFunction.insert(generatedDistFunction.begin(), temp);
    }
    while (origDistFunction[origDistFunction.size() - 1][0] <
           generatedDistFunction[generatedDistFunction.size() - 1][0]) { //add elements at the end of origDistFunction
        vector<double> temp;
        double i = origDistFunction[origDistFunction.size() - 1][0] + 1;
        temp.push_back(i);
        temp.push_back(1.0f);
        origDistFunction.push_back(temp);
    }
    while (generatedDistFunction[generatedDistFunction.size() - 1][0] <
           origDistFunction[origDistFunction.size() - 1][0]) { //add elements at the end of generatedDistFunction
        vector<double> temp;
        double i = generatedDistFunction[generatedDistFunction.size() - 1][0] + 1;
        temp.push_back(i);
        temp.push_back(1.0f);
        generatedDistFunction.push_back(temp);
    }

    //calculate ks statistic
    for (int i = 0; i < origDistFunction.size(); i++) {
        double dnew = fabs(origDistFunction[i][1] - generatedDistFunction[i][1]);
        if (dnew > d) {
            d = dnew;
        }
    }

    if (d <= decider) { //nullhypothesis can not be refused
        cout << "KS-Statistics: " << "Kritischer Wert: " << decider << " d: " << d << " m: " << m << " n: " << n
             << endl;
        return true;
    } else { //nullhypothesis refused
        cout << d << endl;
        dvalue = d;
        return false;
    }

}


vector<double> BruteForceParser::checkResult(vector<vector<double>> avgburstsizes, double origburstsize,
                                             vector<vector<double>> possibleParams, double origLoss,
                                             double origgoodsize) {
    double lowestdiff = 1000;
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

