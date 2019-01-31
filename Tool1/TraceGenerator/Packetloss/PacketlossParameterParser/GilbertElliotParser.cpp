#include <algorithm>
#include "GilbertElliotParser.h"
#include "../PaketlossModel/GilbertElliotModel.h"

double *GilbertElliotParser::parseParameter(vector<bool> trace, unsigned int gMin) {
    return this->bruteForceParameter(trace);
}

double *GilbertElliotParser::bruteForceParameter(vector<bool> trace) {
    double origLoss, avgOrigburstsize, avgOriggoodsize;
    vector<int> origSizes;
    calcLoss(trace, origLoss, avgOrigburstsize, avgOriggoodsize,
             origSizes); //Calculate lossrate and burstsize of the original Trace
    sort(origSizes.begin(), origSizes.end());
    vector<vector<double> > origDistFunction;
    calcDistFunction(origSizes, origDistFunction);
    vector<vector<double> > possibleParams;

    for (int p = 1; p < 51; p++) {
        for (int r = 50; r < 101; r++) {
            for (int h = 1; h < 51; h++) {
                for (int k = 50; k < 100; k++) {
                    double pf = (double) p / 100;
                    double rf = (double) r / 100;
                    double hf = (double) h / 100;
                    double kf = (double) k / 100;
                    double theoreticalLoss = (((1.0f - kf) * (rf / (pf + rf))) + ((1 - hf) * (pf / (pf + rf)))) * 100;
                    double theoreticalavgBurstLength =
                            (1.0f / (1.0f - (1.0f - rf) * (1.0f - hf))) * (1.0f + (1.0f - kf));
                    double avgBurstDiff = fabs(theoreticalavgBurstLength - avgOrigburstsize);
                    if (fabs(theoreticalLoss - origLoss) < 0.1 && avgBurstDiff < 720) {
                        vector<double> params;
                        params.push_back(theoreticalLoss);
                        params.push_back(theoreticalavgBurstLength);
                        params.push_back(avgBurstDiff);
                        params.push_back(pf);
                        params.push_back(rf);
                        params.push_back(kf);
                        params.push_back(hf);
                        possibleParams.push_back(params);
                    }
                }
            }
        }
    }
    cout << possibleParams.size() << endl;
    double p = 0, r = 0, k = 0, h = 0;
    //Filter 50 best fitting parameter from possibleParams
    vector<vector<double> > top50;
    findTopX(top50, possibleParams, 50, "GilbertElliot");

    //Generate for those 50 parameters a trace which is as long as the initial input trace
    bool found = false;
    vector<double> dvalues;
    for (int i = 0; i < top50.size(); i++) {
        cout << " p: " << top50[i][0] << " r: " << top50[i][1] << " k: " << top50[i][2] << " h: " << top50[i][3]
             << endl;
        vector<int> generatedSizes = GilbertElliotModel(trace.size(), top50[i][0], top50[i][1], top50[i][2],
                                                        top50[i][3]).buildTrace2();
        //calculate distributionfunction

        sort(generatedSizes.begin(), generatedSizes.end());
        vector<vector<double> > generatedDistFunction;
        calcDistFunction(generatedSizes, generatedDistFunction);
        //calculate ks test
        double dvalue = 0.0;
        bool ksdecision = kstest(origDistFunction, generatedDistFunction, origSizes.size(), generatedSizes.size(),
                                 dvalue);
        if (ksdecision) {
            //cout << "Parameters found: " << "p: " << top50[i][0] << " r: " << top50[i][1] << " h: " << top50[i][2] << endl;
            found = true;
            p = top50[i][0];
            r = top50[i][1];
            k = top50[i][2];
            h = top50[i][3];
            break;
        } else {
            dvalues.push_back(dvalue);
        }
    }
    if (!found) {
        cout << "No parameters exist that pass the ks-test" << endl;
        int minindex = 0;
        double minvalue = dvalues[0];
        for (int i = 0; i < dvalues.size() - 1; i++) {
            if (dvalues[minindex] > dvalues[i + 1]) {
                minindex = i + 1;
                minvalue = dvalues[i + 1];
            }
        }
        p = top50[minindex][0];
        r = top50[minindex][1];
        k = top50[minindex][2];
        h = top50[minindex][3];
        cout << "dwert: " << minvalue << endl;
    }

    return new double[4]{p, r, k, h};
}
