#include <cmath>
#include "GilbertParser.h"
#include "../PaketlossModel/GilbertElliotModel.h"

float *GilbertParser::parseParameter(vector<bool> trace) {
    return this->bruteForceParameter(trace);
}

float *GilbertParser::bruteForceParameter(vector<bool> trace) {
    float origLoss, avgOrigburstsize, avgOriggoodsize;
    vector<int> origSizes;
    calcLoss(trace, origLoss, avgOrigburstsize, avgOriggoodsize, origSizes); //Calculate lossrate and burstsize of the original Trace
    sort(origSizes.begin(), origSizes.end());
    vector<vector<float> > origDistFunction;
    calcDistFunction(origSizes, origDistFunction);
    vector<vector<float> > possibleParams;
    cout << avgOrigburstsize << endl;
    for (int p = 1; p < 51; p++) {
        for (int r = 50; r < 101; r++) {
            for (int h = 1; h < 51; h++) {
                float pf = (float) p / 100;
                float rf = (float) r / 100;
                float hf = (float) h / 100;
                float theoreticalLoss = ((1-hf)*(pf/(pf+rf)))*100;
                float theoreticalavgBurstLength = 1/(1-(1-rf)*(1-hf));
                float avgBurstDiff = fabs(theoreticalavgBurstLength-avgOrigburstsize);
                if(fabs(theoreticalLoss-origLoss)<0.1 && avgBurstDiff < 720){
                    vector<float> params;
                    params.push_back(theoreticalLoss);
                    params.push_back(theoreticalavgBurstLength);
                    params.push_back(avgBurstDiff);
                    params.push_back(pf);
                    params.push_back(rf);
                    params.push_back(hf);
                    possibleParams.push_back(params);
                }
            }
        }
    }

    float p=0, r=0, k=1, h=0;

    //Filter 50 best fitting parameter from possibleParams
    vector<vector<float> > top50;
    findTopX(top50, possibleParams, 50, "Gilbert");
    //Generate for those 50 parameters a trace which is as long as the initial input trace
    bool found = false;
    vector<float> dvalues;
    for(int i = 0; i<top50.size(); i++){
        vector<int> generatedSizes = GilbertElliotModel(trace.size(), top50[i][0], top50[i][1], 1.0, top50[i][2]).buildTrace2();
        //calculate distributionfunction

        sort(generatedSizes.begin(), generatedSizes.end());
        vector<vector<float> > generatedDistFunction;
        calcDistFunction(generatedSizes, generatedDistFunction);
        //calculate ks test
        float dvalue = 0.0;
        bool ksdecision = kstest(origDistFunction, generatedDistFunction, origSizes.size(), generatedSizes.size(), dvalue);
        if(ksdecision){
            found = true;
            p=top50[i][0];
            r=top50[i][1];
            h=top50[i][2];
            break;
        }else{
            dvalues.push_back(dvalue);
        }
    }
    if(!found){
        cout << "No parameters exist that pass the ks-test" << endl;
        int minindex = 0;
        float minvalue = dvalues[0];
        for(int i = 0; i < dvalues.size()-1; i++){
            if(dvalues[minindex]>dvalues[i+1]){
                minindex = i+1;
                minvalue = dvalues[i+1];
            }
        }
        p=top50[minindex][0];
        r=top50[minindex][1];
        h=top50[minindex][3];
        cout << "dwert: " << minvalue << endl;
    }

    return new float[4] {p, r, 1, h};
}

float *GilbertParser::estimateParameter(vector<bool> trace) {
    float a, b, c = 0;
    float r, h, p;
    unsigned long lossCount = 0;
    unsigned long lossAfterLossCount = 0;
    unsigned long threeLossesCount = 0;
    unsigned long lossRecieveLossCount = 0;
    vector<unsigned long> burstLengths;
    unsigned long currentBurstLength = 0;

    for (unsigned long i = 0; i < trace.size(); i++) {
        if (!trace[i]) {
            lossCount++;
            currentBurstLength++;
            if (i != 0) {
                if (!trace[i - 1]) {
                    lossAfterLossCount++;

                    if (i >= 2) {
                        if (!trace[i - 2]) {
                            threeLossesCount++;
                        }
                    }
                } else if (i >= 2) {
                    if (!trace[i - 2]) {
                        lossRecieveLossCount++;
                    }
                }
            }
        } else {
            if (currentBurstLength > 0) {
                burstLengths.push_back(currentBurstLength);
                currentBurstLength = 0;
            }
        }
    }

    unsigned long burstSum = 0;
    for (unsigned long burst : burstLengths) {
        burstSum += burst;
    }

    float avrgBurstLength = (float) burstSum / (float) burstLengths.size();

    a = 1.f / (float) trace.size() * (float) lossCount;
    b = 1.f / lossCount * (float) lossAfterLossCount;
    c = 1.f / (lossRecieveLossCount + threeLossesCount) * threeLossesCount;

    r = 1 - (a * c - b * b) / (2 * a * c - (b * (a + c)));

    //r = 1.f / avrgBurstLength;

    h = 1.f - (b / (1.f - r));
    p = (a * r) / (1.f - h - a);

    return new float[4]{p, r, 1, h};
}