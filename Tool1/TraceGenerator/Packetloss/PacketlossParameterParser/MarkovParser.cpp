#include "MarkovParser.h"
#include "../PaketlossModel/MarkovModel.h"

double *MarkovParser::parseParameter(vector<bool> trace, unsigned int gMin) {
    return this->bruteForceParameter(trace);
}

double *MarkovParser::bruteForceParameter(vector<bool> trace) {
    const double STEP_SIZE = 0.01f;
    double origLoss, avgOrigburstsize, avgOriggoodsize;
    vector<int> origSizes;
    calcLoss(trace, origLoss, avgOrigburstsize, avgOriggoodsize, origSizes); //Calculate lossrate and burstsize of the original Trace
    sort(origSizes.begin(), origSizes.end());
    vector<vector<double> > origDistFunction;
    calcDistFunction(origSizes, origDistFunction);
    vector<vector<double> > possibleParams;
    double p13_estimated = -1.0f;
    double p31_estimated = -1.0f;
    double p32_estimated = -1.0f;
    double p23_estimated = -1.0f;
    double p14_estimated = -1.0f;
    double p41 = 1.0f;
    for(int p14_i = 1; p14_i < 10; p14_i++){
        for(int p13_i = 1; p13_i < 100-p14_i; p13_i++){
            for(int p32_i = 1; p32_i < 30; p32_i++){
                for(int p31_i = 1; p31_i <= 100 - p32_i; p31_i++){
                    for(int p23_i = 80; p23_i < 100; p23_i++){
                        double p14 = (double)p14_i/100;
                        double p13 = (double)p13_i/100;
                        double p32 = (double)p32_i/100;
                        double p31 = (double)p31_i/100;
                        double p23 = (double)p23_i/100;
                        double S4 = 1.0f/(1.0f+(p41/p14)+((p41*p13)/(p14*p31))+((p41*p13*p32)/(p14*p31*p23))); //Steady-State-Probability S4
                        double S1 = 1.0f/(1.0f+(p14/p41)+(p13/p31)+((p13*p32)/(p31*p23))); //Steady-State-Probability S1
                        double S3 = 1.0f/(1.0f+(p32/p23)+(p31/p13)+((p14*p31)/(p41*p13))); //Steady-State-Probability S3
                        double S2 = 1.0f/(1.0f+(p23/p32)+((p31*p23)/(p13*p32))+((p14*p31*p23)/(p41*p13*p32))); //Steady-State-Probability S2
                        double theoreticalLoss = (S4+S3)*100;
                        double theoreticalavgBurstLength = (S4+S3)/(S1*(p14+p13)+(S2*p23));
                        double avgBurstDiff = fabs(theoreticalavgBurstLength-avgOrigburstsize);
                        if(fabs(theoreticalLoss-origLoss) < 0.1 && avgBurstDiff < 720){
                            vector<double> params;
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
    //Filter 50 best fitting parameter from possibleParams
    vector<vector<double> > top50;
    findTopX(top50, possibleParams, 50, "Markov");

    //Generate for those 50 parameters a trace which is as long as the initial input trace
    bool found = false;
    vector<double> dvalues;
    for(int i = 0; i<top50.size(); i++){
        vector<int> generatedSizes = MarkovModel(trace.size(), top50[i][0], top50[i][1], top50[i][2], top50[i][3], top50[i][4]).buildTrace2();
        //calculate distributionfunction
        sort(generatedSizes.begin(), generatedSizes.end());
        vector<vector<double> > generatedDistFunction;
        calcDistFunction(generatedSizes, generatedDistFunction);

        //calculate ks test
        double dvalue = 0.0;
        bool ksdecision = kstest(origDistFunction, generatedDistFunction, origSizes.size(), generatedSizes.size(), dvalue);
        if(ksdecision){
            found = true;
            p13_estimated=top50[i][0];
            p31_estimated=top50[i][1];
            p32_estimated=top50[i][2];
            p23_estimated=top50[i][3];
            p14_estimated=top50[i][4];
            break;
        }else{
            dvalues.push_back(dvalue);
        }
    }
    if(!found){
        cout << "No parameters exist that pass the ks-test" << endl;
        int minindex = 0;
        double minvalue = dvalues[0];
        for(int i = 0; i < dvalues.size()-1; i++){
            if(dvalues[minindex]>dvalues[i+1]){
                minindex = i+1;
                minvalue = dvalues[i+1];
            }
        }
        p13_estimated=top50[minindex][0];
        p31_estimated=top50[minindex][1];
        p32_estimated=top50[minindex][2];
        p23_estimated=top50[minindex][3];
        p14_estimated=top50[minindex][4];
        cout << "dwert: " << minvalue << endl;
    }

    return new double[6] {p13_estimated, p31_estimated, p32_estimated, p23_estimated, p14_estimated, p41};
}
