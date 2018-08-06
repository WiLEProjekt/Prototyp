#include <cmath>
#include "GilbertParser.h"
#include "../PaketlossModel/GilbertElliotModel.h"

float *GilbertParser::parseParameter(vector<bool> trace) {
    return this->estimateParameter(trace);
}

float *GilbertParser::bruteForceParameter(vector<bool> trace) {
    float origLoss, avgOrigburstsize, avgOriggoodsize;
    vector<int> origSizes;
    calcLoss(origTrace, origLoss, avgOrigburstsize, avgOriggoodsize, origSizes); //Calculate lossrate and burstsize of the original Trace
    cout << "avg burstsize: " << avgOrigburstsize << endl;
    sort(origSizes.begin(), origSizes.end());
    vector<vector<float> > origDistFunction;
    calcDistFunction(origSizes, origDistFunction);
    vector<vector<float> > possibleParams;
    vector<vector<float> > avgburstsizes;
    for (int p = 1; p < 51; p++) {
        for (int r = 50; r < 101; r++) {
            for (int h = 1; h < 51; h++) {
                float pf = (float) p / 100;
                float rf = (float) r / 100;
                float hf = (float) h / 100;
                float theoreticalLoss = (1-hf)*(pf/(pf+rf))*100;
                float theoreticalavgBurstLength = 1/(1-(1-rf)*(1-hf));
                float avgBurstDiff = fabs(theoreticalavgBurstLength-avgOrigburstsize);
                if(fabs(theoreticalLoss-origLoss)<0.1 && avgBurstDiff < 0.1){
                    vector<float> params;
                    params.push_back(pf);
                    params.push_back(rf);
                    params.push_back(hf);
                    params.push_back(theoreticalLoss);
                    params.push_back(theoreticalavgBurstLength);
                    params.push_back(avgBurstDiff);
                    possibleParams.push_back(params);
                }
            }
        }
    }

    //Filter 50 best fitting parameter from possibleParams
    vector<vector<float> > top50;
    for(int a = 0; a<50; a++){
        int minindex = 0;
        for(int i = 0; i<possibleParams.size(); i++){
            if(possibleParams[i][5]<possibleParams[minindex][5]){
                minindex = i;
            }
        }
        vector<float>tmp;
        tmp.push_back(possibleParams[minindex][0]);
        tmp.push_back(possibleParams[minindex][1]);
        tmp.push_back(possibleParams[minindex][2]);
        top50.push_back(tmp);
        possibleParams.erase(possibleParams.begin()+minindex);
    }



    /*
    float origLoss, origburstsize, origgoodsize;
    calcLoss(trace, origLoss, origburstsize, origgoodsize); //Calculate lossrate of the original Trace
    vector<vector<float> > possibleParams;
    vector<vector<float> > avgburstsizes;
    for (int p = 1; p < 51; p++) {
        for (int r = 50; r < 101; r++) {
            for (int h = 1; h < 51; h++) {
                float pf = (float) p / 100;
                float rf = (float) r / 100;
                float hf = (float) h / 100;
                float theoreticalLoss = (1 - hf) * (pf / (pf + rf)) * 100;
                if (fabs(theoreticalLoss - origLoss) < 0.1) {
                    vector<float> params;
                    params.push_back(pf);
                    params.push_back(rf);
                    params.push_back(hf);
                    possibleParams.push_back(params);
                }
            }
        }
    }

    //PERFORMANCE VERBESSERUNG: VIELLEICHT ITERATIV, ERST NUR 20000 PAKETE TESTEN, DANN DIE BESTEN 20ERGEBNISSE NEHMEN UND NOCHMAL MIT 200000 PAKETEN TESTEN
    for (int i = 0; i < possibleParams.size(); i++) {
        GilbertElliotModel(20000, possibleParams[i][0], possibleParams[i][1], 1.0, possibleParams[i][2]).buildTrace(avgburstsizes);
    }

    vector<float> bestParams = checkResult(avgburstsizes, origburstsize, possibleParams, origLoss, origgoodsize);
    return new float[4] {bestParams[0], bestParams[1], 1, bestParams[3]};*/
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