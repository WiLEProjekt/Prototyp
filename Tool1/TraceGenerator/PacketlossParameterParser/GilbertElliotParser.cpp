#include <algorithm>
#include "GilbertElliotParser.h"
#include "../PaketlossModel/GilbertElliotModel.h"

float *GilbertElliotParser::parseParameter(vector<bool> trace, unsigned int gMin) {
    return this->bruteForceParameter(trace);
}

float *GilbertElliotParser::estimateParameter(vector<bool> trace, unsigned int gMin) {
    if (gMin == 0) {

        gMin = 16;
    }
    vector<int> lossindices;
    vector<int> gapindices;
    for (int i = 0; i < trace.size(); i++) { //find all loss indices
        if (!trace[i]) {
            lossindices.push_back(i);
        }
    }
    for (int i = 1; i < lossindices.size(); i++) { //push
        if (lossindices[i] - lossindices[i - 1] - 1 < gMin) {
            if (!(find(gapindices.begin(), gapindices.end(), lossindices[i - 1]) != gapindices.end())) {
                gapindices.push_back(lossindices[i - 1]);
            }
            if (!(find(gapindices.begin(), gapindices.end(), lossindices[i]) != gapindices.end())) {
                gapindices.push_back(lossindices[i]);
            }
        }
    }

    vector<int> bursts; //startindex and endindex
    for (int i = 0; i < gapindices.size(); i++) {
        if (i == 0) {
            bursts.push_back(gapindices[i]);
        } else {
            if ((gapindices[i] - gapindices[i - 1]) - 1 >= gMin) {
                bursts.push_back(gapindices[i - 1]);
                bursts.push_back(gapindices[i]);
            }
            if (i == gapindices.size() - 1) {
                bursts.push_back(gapindices[i]);
            }
        }
    }
    if (bursts.size() > 0) {
        if ((trace.size() - 1) - (bursts[bursts.size() - 1]) < gMin) {
            bursts[bursts.size() - 1] = trace.size() - 1;
        }
    }

    int transingoodstate = 0;
    int transinbadstate = 0;
    if (bursts.size() == 0) {
        transingoodstate = trace.size() - 1;
    } else {
        for (int i = 0; i < bursts.size(); i++) {
            if (i % 2 == 0) {//even = start index of burst state found
                if (i == 0) {
                    transingoodstate = transingoodstate + bursts[i];
                } else {
                    transingoodstate = transingoodstate + (bursts[i] - bursts[i - 1] - 1);
                }
            } else { //odd = end index of burst state
                transinbadstate = transinbadstate + (bursts[i] - bursts[i - 1] + 1);
            }
        }
        if (bursts[bursts.size() - 1] < trace.size() - 1) {
            transingoodstate = transingoodstate + (trace.size() - 1 - bursts[bursts.size() - 1] - 1);
        }
    }

    int goodBadStateTrans = 0;
    int badGoodStateTrans = 0;
    if (bursts.size() > 0) {
        if (bursts[0] != 0 && bursts[bursts.size() - 1] != trace.size() - 1) {
            goodBadStateTrans = bursts.size() / 2;
            badGoodStateTrans = bursts.size() / 2;
        } else if (bursts[0] == 0 && bursts[bursts.size() - 1] != trace.size() - 1) {
            goodBadStateTrans = bursts.size() / 2 - 1;
            badGoodStateTrans = bursts.size() / 2;
        } else if (bursts[0] != 0 && bursts[bursts.size() - 1] == trace.size() - 1) {
            goodBadStateTrans = bursts.size() / 2;
            badGoodStateTrans = bursts.size() / 2 - 1;
        } else {
            goodBadStateTrans = bursts.size() / 2 - 1;
            badGoodStateTrans = bursts.size() / 2 - 1;
        }
    }

    float p = (float) goodBadStateTrans / (float) transingoodstate;
    float r = (float) badGoodStateTrans / (float) transinbadstate;

//calculate k, h
    int rcvinGoodState = 0;
    int lstinGoodState = 0;
    int rcvinBadState = 0;
    int lstinBadState = 0;
    for (int i = 0; i < bursts.size(); i++) {
        if (i % 2 == 0) {
            if (i == 0) {
                for (int a = 0; a < bursts[i]; a++) {
                    if (trace[a] == 1) {
                        rcvinGoodState++;
                    } else {
                        lstinGoodState++;
                    }
                }
            } else {
                for (int a = bursts[i - 1] + 1; a < bursts[i]; a++) {
                    if (trace[a] == 1) {
                        rcvinGoodState++;
                    } else {
                        lstinGoodState++;
                    }
                }
            }

        } else {
            for (int a = bursts[i - 1]; a <= bursts[i]; a++) {
                if (trace[a] == 1) {
                    rcvinBadState++;
                } else {
                    lstinBadState++;
                }
            }
        }
    }
    if (bursts.size() > 0) {
        if (bursts[bursts.size() - 1] < trace.size() - 1) {
            for (int a = bursts[bursts.size() - 1] + 1; a < trace.size(); a++) {
                if (trace[a] == 1) {
                    rcvinGoodState++;
                } else {
                    lstinGoodState++;
                }
            }
        }
    } else {
        for (int i = 0; i < trace.size(); i++) {
            if (trace[i]) {
                rcvinGoodState++;
            } else {
                lstinGoodState++;
            }
        }
    }
    float k = (float) rcvinGoodState / (float) (rcvinGoodState + lstinGoodState);
    float h = (float) rcvinBadState / (float) (rcvinBadState + lstinBadState);

    return new float[4]{p, r, k, h};
}

float *GilbertElliotParser::bruteForceParameter(vector<bool> trace) {
    float origLoss, avgOrigburstsize, avgOriggoodsize;
    vector<int> origSizes;
    calcLoss(trace, origLoss, avgOrigburstsize, avgOriggoodsize, origSizes); //Calculate lossrate and burstsize of the original Trace
    sort(origSizes.begin(), origSizes.end());
    vector<vector<float> > origDistFunction;
    calcDistFunction(origSizes, origDistFunction);
    vector<vector<float> > possibleParams;

    for (int p = 1; p < 51; p++) {
        for (int r = 50; r < 101; r++) {
            for (int h = 1; h < 51; h++) {
                for (int k = 50; k < 100; k++){
                    float pf = (float) p / 100;
                    float rf = (float) r / 100;
                    float hf = (float) h / 100;
                    float kf = (float) k / 100;
                    float theoreticalLoss = (((1.0f-kf)*(rf/(pf+rf)))+((1-hf)*(pf/(pf+rf))))*100;
                    float theoreticalavgBurstLength = (1.0f/(1.0f-(1.0f-rf)*(1.0f-hf)))*(1.0f+(1.0f-kf));
                    float avgBurstDiff = fabs(theoreticalavgBurstLength-avgOrigburstsize);
                    if(fabs(theoreticalLoss-origLoss)<0.1 && avgBurstDiff < 720){
                        vector<float> params;
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
    float p=0, r=0, k=0, h=0;
    //Filter 50 best fitting parameter from possibleParams
    vector<vector<float> > top50;
    findTopX(top50, possibleParams, 50, "GilbertElliot");

    //Generate for those 50 parameters a trace which is as long as the initial input trace
    bool found = false;
    vector<float> dvalues;
    for(int i = 0; i<top50.size(); i++){
        cout <<" p: " << top50[i][0] << " r: " << top50[i][1] << " k: " << top50[i][2] <<" h: " << top50[i][3] << endl;
        vector<int> generatedSizes = GilbertElliotModel(trace.size(), top50[i][0], top50[i][1], top50[i][2], top50[i][3]).buildTrace2();
        //calculate distributionfunction

        sort(generatedSizes.begin(), generatedSizes.end());
        vector<vector<float> > generatedDistFunction;
        calcDistFunction(generatedSizes, generatedDistFunction);
        //calculate ks test
        float dvalue = 0.0;
        bool ksdecision = kstest(origDistFunction, generatedDistFunction, origSizes.size(), generatedSizes.size(), dvalue);
        if(ksdecision){
            //cout << "Parameters found: " << "p: " << top50[i][0] << " r: " << top50[i][1] << " h: " << top50[i][2] << endl;
            found = true;
            p=top50[i][0];
            r=top50[i][1];
            k=top50[i][2];
            h=top50[i][3];
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
        k=top50[minindex][2];
        h=top50[minindex][3];
        cout << "dwert: " << minvalue << endl;
    }

    return new float[4] {p, r, k, h};
}
