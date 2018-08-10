#include "MarkovParser.h"
#include "../PaketlossModel/MarkovModel.h"

float *MarkovParser::parseParameter(vector<bool> trace, unsigned int gMin) {
    return this->bruteForceParameter(trace);
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
    const float STEP_SIZE = 0.01f;
    float origLoss, avgOrigburstsize, avgOriggoodsize;
    vector<int> origSizes;
    calcLoss(trace, origLoss, avgOrigburstsize, avgOriggoodsize, origSizes); //Calculate lossrate and burstsize of the original Trace
    cout << "burst size: " << avgOrigburstsize << " loss: " << origLoss << endl;
    sort(origSizes.begin(), origSizes.end());
    vector<vector<float> > origDistFunction;
    calcDistFunction(origSizes, origDistFunction);
    vector<vector<float> > possibleParams;
    float p13_estimated = -1.0f;
    float p31_estimated = -1.0f;
    float p32_estimated = -1.0f;
    float p23_estimated = -1.0f;
    float p14_estimated = -1.0f;
    float p41 = 1.0f;
    for(int p14_i = 1; p14_i < 10; p14_i++){
        for(int p13_i = 1; p13_i < 100-p14_i; p13_i++){
            for(int p32_i = 1; p32_i < 30; p32_i++){
                for(int p31_i = 1; p31_i <= 100 - p32_i; p31_i++){
                    for(int p23_i = 80; p23_i < 100; p23_i++){
                        float p14 = (float)p14_i/100;
                        float p13 = (float)p13_i/100;
                        float p32 = (float)p32_i/100;
                        float p31 = (float)p31_i/100;
                        float p23 = (float)p23_i/100;
                        float S4 = 1.0f/(1.0f+(p41/p14)+((p41*p13)/(p14*p31))+((p41*p13*p32)/(p14*p31*p23))); //Steady-State-Probability S4
                        float S1 = 1.0f/(1.0f+(p14/p41)+(p13/p31)+((p13*p32)/(p31*p23))); //Steady-State-Probability S1
                        float S3 = 1.0f/(1.0f+(p32/p23)+(p31/p13)+((p14*p31)/(p41*p13))); //Steady-State-Probability S3
                        float S2 = 1.0f/(1.0f+(p23/p32)+((p31*p23)/(p13*p32))+((p14*p31*p23)/(p41*p13*p32))); //Steady-State-Probability S2
                        float theoreticalLoss = (S4+S3)*100;
                        float theoreticalavgBurstLength = (S4+S3)/(S1*(p14+p13)+(S2*p23));
                        float avgBurstDiff = fabs(theoreticalavgBurstLength-avgOrigburstsize);

                        if(p13_i == 20 && p31_i ==70 && p32_i == 10 && p23_i == 90 && p14_i == 5){
                            cout << p14 << " " << p13 << " " << p32 << " " << p31 << " " << p23 << endl;
                            cout << "theo burst size: " << theoreticalavgBurstLength << " theo loss: " << theoreticalLoss << " avgBurstDiff: " << avgBurstDiff << endl;
                        }

                        if(fabs(theoreticalLoss-origLoss) < 0.1 && avgBurstDiff < 0.001){
                            vector<float> params;
                            params.push_back(theoreticalLoss);
                            params.push_back(theoreticalavgBurstLength);
                            params.push_back(avgBurstDiff);
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
    }
    cout << possibleParams.size() << endl;
    /*for(int i = 0; i<possibleParams.size(); i++){
        if(possibleParams[i][3] == 0.2){ //??????
            cout << "p13: " << possibleParams[i][3] << " p31: " << possibleParams[i][4] << " p32: " << possibleParams[i][5] << " p23: " << possibleParams[i][6] << " p14: " << possibleParams[i][7] << endl;
        }
    }*/
    //Filter 50 best fitting parameter from possibleParams
    vector<vector<float> > top50;
    findTopX(top50, possibleParams, 50, "Markov");


    //for(int i = 0; i < top50.size(); i++){
    //    cout << "p13: " << top50[i][0] << " p31: " << top50[i][1] << " p32: " << top50[i][2] << " p23: " << top50[i][3] << " p14: " << top50[i][4] << " burstsize: " << top50[i][5] << endl;
    //}

    //Generate for those 50 parameters a trace which is as long as the initial input trace
    bool found = false;
    for(int i = 0; i<possibleParams.size(); i++){
        vector<int> generatedSizes = MarkovModel(trace.size(), possibleParams[i][3], possibleParams[i][4], possibleParams[i][5], possibleParams[i][6], possibleParams[i][7]).buildTrace2();
        cout << generatedSizes.size() << endl; //Vermutlich bei Markov-Generierung ist was falsch
        //calculate distributionfunction
        sort(generatedSizes.begin(), generatedSizes.end());
        vector<vector<float> > generatedDistFunction;
        calcDistFunction(generatedSizes, generatedDistFunction);

        //calculate ks test
        bool ksdecision = kstest(origDistFunction, generatedDistFunction, origSizes.size(), generatedSizes.size());
        if(ksdecision){
            //cout << "Parameters found: " << "p: " << top50[i][0] << " r: " << top50[i][1] << " h: " << top50[i][2] << endl;
            found = true;

            p13_estimated=top50[i][0];
            p31_estimated=top50[i][1];
            p32_estimated=top50[i][2];
            p23_estimated=top50[i][3];
            p14_estimated=top50[i][4];
            p13_estimated=possibleParams[i][3];
            p31_estimated=possibleParams[i][4];
            p32_estimated=possibleParams[i][5];
            p23_estimated=possibleParams[i][6];
            p14_estimated=possibleParams[i][7];
            break;
        }
    }
    if(!found){
        cout << "No matching parameters found" << endl;
    }

    return new float[6] {p13_estimated, p31_estimated, p32_estimated, p23_estimated, p14_estimated, p41};
}
