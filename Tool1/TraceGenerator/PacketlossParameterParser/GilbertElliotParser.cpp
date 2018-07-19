//
// Created by dennis on 18.07.18.
//

#include <algorithm>
#include "GilbertElliotParser.h"

float *GilbertElliotParser::parseParameter(vector<bool> trace, unsigned int gMin) {
    return this->estimateParameter(trace, gMin);
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
    return nullptr;
}
