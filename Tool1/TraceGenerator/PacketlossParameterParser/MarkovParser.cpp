#include "MarkovParser.h"
#include "../PaketlossModel/MarkovModel.h"

float *MarkovParser::parseParameter(vector<bool> trace, unsigned int gMin) {
    return this->estimateParameter(trace, gMin);
}

float *MarkovParser::estimateParameter(vector<bool> trace, unsigned int gMin) {
    if (gMin == 0) {
        gMin = 4;
    }
    const unsigned int B_MIN = 1;
    unsigned long lossCounter = 0;
    unsigned long receiveCounter = 0;
    bool gapPeriod = true;

    //first index of the burst, length of the burst
    map<unsigned long, unsigned long> bursts;
    //first index of the gap, length of the gap
    map<unsigned long, unsigned long> gaps;

    //Collect all gap and burst periods
    unsigned long periodLength = 0;
    for (unsigned long i = 0; i < trace.size(); i++) {
        if (!trace[i]) {
            lossCounter++;
        } else {
            receiveCounter++;
        }
        periodLength = receiveCounter + lossCounter;
        if (gapPeriod) {
            //Check if gapPeriod
            if (i > B_MIN) {
                gapPeriod = false;
                for (int j = 0; j < B_MIN; j++) {
                    if (trace[i - j]) {
                        gapPeriod = true;
                    }
                }
            }
            if (!gapPeriod) {
                gaps.insert(pair<unsigned long, unsigned long>(i - periodLength + 1, periodLength));
                receiveCounter = 0;
                lossCounter = 0;
            }
        } else {
            //Check if gapPeriod
            if (i > gMin) {
                gapPeriod = true;
                for (int j = 0; j < gMin; j++) {
                    if (!trace[i - j]) {
                        gapPeriod = false;
                    }
                }
            }
            if (gapPeriod) {
                bursts.insert(pair<unsigned long, unsigned long>(i - periodLength + 1, periodLength));
                receiveCounter = 0;
                lossCounter = 0;
            }
        }
    }
    //collect last period
    if (gapPeriod) {
        gaps.insert(pair<unsigned long, unsigned long>(trace.size() - periodLength, periodLength));
    } else {
        bursts.insert(pair<unsigned long, unsigned long>(trace.size() - periodLength, periodLength));
    }

    unsigned long state1Counter = 0;
    unsigned long state2Counter = 0;
    unsigned long state3Counter = 0;
    unsigned long state4Counter = 0;

    unsigned long from3To2Counter = 0;
    unsigned long from2To3Counter = 0;
    unsigned long from1To4Counter = 0;
    unsigned long from4To1Counter = 0;
    unsigned long from1To3Counter = 0;
    unsigned long from3To1Counter = 0;

    //State-changes in burst period
    for (auto &burst : bursts) {
        for (unsigned long i = 0; i < burst.second; i++) {
            if (trace[burst.first + i]) {
                state2Counter++;
                if (i > 0) {
                    //Case 1: packet received after loss: 3->2
                    if (!trace[burst.first + i - 1]) {
                        from3To2Counter++;
                    }
                }
            } else {
                state3Counter++;
                if (i != 0) {
                    //Case 2: packet loss after receive: 2->3
                    if (trace[burst.first + i - 1]) {
                        from2To3Counter++;
                    }
                }
            }
        }
    }

    //State-changes in gap period
    for (auto &gap : gaps) {
        for (unsigned long i = 1; i < gap.second; i++) {
            if (trace[gap.first + i]) {
                state1Counter++;
                //Case 1: packet received after loss: 4->1
                if (!trace[gap.first + i - 1]) {
                    from4To1Counter++;
                }
            } else {
                state4Counter++;
                //Case 2: packet loss after receive: 1->4
                if (trace[gap.first + i - 1]) {
                    from1To4Counter++;
                }
            }
        }
    }

    //State-changes between periods
    from1To3Counter = bursts.size();
    from3To1Counter = gaps.size() - 1;


    float p32 = 1.f / (float) state3Counter * from3To2Counter;
    float p23 = 1.f / (float) state2Counter * from2To3Counter;
    float p14 = 1.f / (float) state1Counter * from1To4Counter;
    float p13 = 1.f / (float) state1Counter * from1To3Counter;
    float p31 = 1.f / (float) state3Counter * from3To1Counter;
    float p41 = 1;

    return new float[6]{p13, p31, p32, p23, p14, p41};
}

float *MarkovParser::bruteForceParameter(vector<bool> trace) {
    /*
    const float STEP_SIZE = 0.01f;
    float origLoss, origburstsize, origgoodsize;
    calcLoss(trace, origLoss, origburstsize, origgoodsize); //Calculate lossrate of the original Trace
    vector<vector<float> > possibleParams;
    vector<vector<float> > avgburstsizes;
    for(float p14 = 0; p14 < 1.0f; p14 += STEP_SIZE){
        for(float p13 = 0; p13 < 1.0f-p14; p13 += STEP_SIZE){
            for(float p32 = 0; p32 < 1.0f; p32 += STEP_SIZE){
                for(float p31 = 0; p31 < 1.0f - p32; p31 += STEP_SIZE){
                    for(float p23 = 0; p23 < 1.0f; p23 += STEP_SIZE){
                        vector<float> params;
                        params.push_back(p13);
                        params.push_back(p31);
                        params.push_back(p32);
                        params.push_back(p23);
                        params.push_back(p14);
                        possibleParams.push_back(params);
                    }
                }
            }
        }
    }

    for(int i = 0; i< possibleParams.size(); i++){
        MarkovModel(20000, possibleParams[i][0], possibleParams[i][1], possibleParams[i][2], possibleParams[i][3], possibleParams[i][4]).buildTrace(avgburstsizes);
    }

    vector<float> bestParams = checkResult(avgburstsizes,origburstsize, possibleParams, origLoss, origgoodsize);

    return new float[5] {bestParams[0],bestParams[1],bestParams[2],bestParams[3],bestParams[4]};*/
}
