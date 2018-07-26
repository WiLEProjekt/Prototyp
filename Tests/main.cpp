#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <math.h>
#include <time.h>
#include <cstring>
#include <algorithm>

using namespace std;
mt19937 generator; //Mersenne Twister Engine
uniform_real_distribution<float> distribution;

float genRandomNumber() {
    return (distribution(generator));
}

vector<bool> readFile(string filename) {
    vector<bool> trace;
    ifstream file;
    file.open(filename, ios::in);
    char c;
    while (file.get(c)) {
        trace.push_back(c != '0');
    }
    return trace;
}

void createGilbertElliotTrace(int packetnumber, float p, float r, float k, float h, vector<vector<float> > &avgburstszs, vector<int> &genSizes) {
    vector<bool> trace;
    vector<int> burstsizes;
    vector<int> goodsizes;
    vector<int> overallSizes;
    bool good = true; // 1 = good state, 0 = bad state
    bool send = true; // 0 = loss, 1 = successfully send, or no loss
    int temp = 0;
    int losscounter = 0;
    int receivecounter = 0;
    for (long i = 0; i < packetnumber + 1; i++) {
        //generate Trace
        if (good) { //in good state
            float randomNumberLossK = genRandomNumber(); //Random Number that indicates if a packet is lost while being in a state
            float randomNumberStateP = genRandomNumber(); //Random number that indicates a state transition from good -> bad
            send = !(randomNumberLossK <= (1.0 - k)); //Calculate next state
            good = !(randomNumberStateP <= p); //Calculate next state
        } else { //in bad state
            float randomNumberLossH = genRandomNumber(); //Random Number that indicates if a packet is lost while being in a state
            float randomNumberStateR = genRandomNumber(); //Random number that indicates a state transition from bad -> good
            send = !(randomNumberLossH <= (1.0 - h));
            good = (randomNumberStateR <= r); //Calculate next state
        }

        if (i != 0) {
            trace.push_back(send);
        }

        //calculate burstsizes
        if(i > 1){
            if(!trace[i-1] && !trace[i-2]){
                losscounter++;
                temp--;
            }else if(trace[i-1] && trace[i-2]){
                receivecounter++;
                temp++;
            }else{
                if(temp < 0){
                    burstsizes.push_back(temp*(-1));
                    genSizes.push_back(temp);
                }else{
                    goodsizes.push_back(temp);
                    genSizes.push_back(temp);
                }
                temp = 0;
                if(!trace[i-1]){
                    losscounter++;
                    temp--;
                }
                else if(trace[i-1]) {
                    receivecounter++;
                    temp++;
                }
            }
        }else if(i==1){
            if(!trace[i-1]){
                losscounter++;
                temp--;
            }
            else if(trace[i-1]){
                receivecounter++;
                temp++;
            }
        }
    }
    vector<float> params;
    float lossrate = (float)losscounter/trace.size()*100;
    float avgBstSize = (float)losscounter/burstsizes.size();
    float avgGoodSize = (float)receivecounter/goodsizes.size();
    params.push_back(lossrate);
    params.push_back(avgBstSize);
    params.push_back(avgGoodSize);
    avgburstszs.push_back(params);
    sort(overallSizes.begin(), overallSizes.end());
    //genSizes.push_back(overallSizes);
}

void calcLoss(vector<bool> &trace, float &result, float &burstsize, float &goodsize, vector<int> &overallsizes) {
    vector<int> burstsizes;
    vector<int> goodsizes;
    int temp = 0;
    int losscounter = 0;
    int receivecounter = 0;
    for (int i = 0; i < trace.size(); i++) {
        if(i > 0){
            if(!trace[i] && !trace[i-1]){
                losscounter++;
                temp--;
            }else if(trace[i] && trace[i-1]){
                receivecounter++;
                temp++;
            }else{
                if(temp < 0){
                    burstsizes.push_back((temp)*(-1));
                    overallsizes.push_back(temp);
                }else{
                    goodsizes.push_back(temp);
                    overallsizes.push_back(temp);
                }
                temp = 0;
                if(!trace[i]){
                    losscounter++;
                    temp--;
                }
                else if(trace[i]) {
                    receivecounter++;
                    temp++;
                }
            }
        }else{
            if(!trace[i]){
                losscounter++;
                temp--;
            }
            else if(trace[i]){
                receivecounter++;
                temp++;
            }
        }
    }
    result = (float) losscounter / trace.size() * 100;
    burstsize = (float) losscounter/burstsizes.size();
    goodsize = (float) receivecounter/goodsizes.size();
}

void calcDistFunction(vector<int> &sizes, vector<vector<float> > &distFunction){
    float cumprob = 0;
    for(int i = sizes[0]; i <= sizes[sizes.size()-1]; i++){
        vector<float> temp;
        int counter = count(sizes.begin(), sizes.end(), i);
        float tmp = (float)i;
        temp.push_back(tmp);
        float prob = cumprob + (float)counter/sizes.size();
        temp.push_back(prob);
        cumprob = prob;
        distFunction.push_back(temp);
    }
}

//Two-sided Kolmogorov-Smirnov Test
float kstest(vector<vector<float> > origDistFunction, vector<vector<float> > &generatedDistFunction, int m, int n){
    float alpha = 0.01; //significanceniveau
    float criticalValue = sqrt((log(2/alpha))/2);
    float multiplier = sqrt((float)(n*m)/(n+m));
    float d; //ks test value
    cout << "old: " << origDistFunction.size() << " " << generatedDistFunction.size() << endl;
    //bring both distribution functions to the same size by adding elements at the beginning and at the end
    while(origDistFunction[0][0] > generatedDistFunction[0][0]){ //add elements at the beginning of origDistFunction
        vector<float> temp;
        float i = origDistFunction[0][0] - 1;
        temp.push_back(i);
        temp.push_back(0.0f);
        origDistFunction.insert(origDistFunction.begin(), temp);
    }
    while(generatedDistFunction[0][0] > origDistFunction[0][0]){ //add elements at the beginning of generatedDistFunction
        vector<float> temp;
        float i = generatedDistFunction[0][0] - 1;
        temp.push_back(i);
        temp.push_back(0.0f);
        generatedDistFunction.insert(generatedDistFunction.begin(), temp);
    }
    while(origDistFunction[origDistFunction.size()-1][0] < generatedDistFunction[generatedDistFunction.size()-1][0]){ //add elements at the end of origDistFunction
        vector<float> temp;
        float i = origDistFunction[origDistFunction.size()-1][0] + 1;
        temp.push_back(i);
        temp.push_back(1.0f);
        origDistFunction.push_back(temp);
    }
    while(generatedDistFunction[generatedDistFunction.size()-1][0] < origDistFunction[origDistFunction.size()-1][0]){ //add elements at the end of generatedDistFunction
        vector<float> temp;
        float i = generatedDistFunction[generatedDistFunction.size()-1][0] + 1;
        temp.push_back(i);
        temp.push_back(1.0f);
        generatedDistFunction.push_back(temp);
    }
    cout << origDistFunction.size() << " " << generatedDistFunction.size() << endl;

}

int main(int argc, char **argv) {
    double time1, tstart1;
    tstart1 = clock(); //Zeitmessung starten
    generator.seed(1);
    uniform_real_distribution<float> *dist = new uniform_real_distribution<float>(0.0, 1.0);
    distribution = *dist;
    if (argc != 2) {
        cout << "usage: ./main filename [gilbert/gilbertelliot/markov]" << endl;
    } else {
        string filename = argv[1];
        vector<bool> origTrace = readFile(filename);
        float origLoss, origburstsize, origgoodsize;
        vector<int> origSizes;
        calcLoss(origTrace, origLoss, origburstsize, origgoodsize, origSizes); //Calculate lossrate and burstsize of the original Trace
        sort(origSizes.begin(), origSizes.end());
        vector<vector<float> > origDistFunction;
        calcDistFunction(origSizes, origDistFunction);
        //for(int i = 0; i<origDistFunction.size(); i++){
        //    cout << origDistFunction[i][0] << " " << origDistFunction[i][1] << endl;
        //}

        //cout << origburstsize << endl;
        vector<vector<float> > possibleParams;
        vector<vector<float> > avgburstsizes;
        //vector<vector<int> > generatedSizes;
        for (int p = 1; p < 51; p++) {
            for (int r = 50; r < 101; r++) {
                for (int h = 1; h < 51; h++) {
                    float pf = (float) p / 100;
                    float rf = (float) r / 100;
                    //float kf = (float) k / 100;
                    float hf = (float) h / 100;
                    float theoreticalLoss = (1-hf)*(pf/(pf+rf))*100;
                    float theoreticalavgBurstLength = 1/(1-(1-rf)*(1-hf));
                    float avgBurstDiff = fabs(theoreticalavgBurstLength-origburstsize);
                    if(fabs(theoreticalLoss-origLoss)<0.1 && avgBurstDiff < 0.05){
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
        //Generate for those 50 parameters a trace which is as long as the initial input trace
        for(int i = 0; i<top50.size(); i++){
            //cout << top50[i][0] << " " << top50[i][1] << " " << top50[i][2] << endl;
            vector<int> generatedSizes;
            createGilbertElliotTrace(origTrace.size(), top50[i][0], top50[i][1], 1.0, top50[i][2], avgburstsizes, generatedSizes);
            //calculate distributionfunction
            sort(generatedSizes.begin(), generatedSizes.end());
            vector<vector<float> > generatedDistFunction;
            calcDistFunction(generatedSizes, generatedDistFunction);
            //TODO: calculate ks test
            float ksvalue = kstest(origDistFunction, generatedDistFunction, origSizes.size(), generatedSizes.size());
            //TODO: calculate p-value

        }
        //cout << possibleParams[minindex][0] << " " << possibleParams[minindex][1] << " " << possibleParams[minindex][2] << endl;





    }
    time1 = clock()-tstart1; //Zeitmessung beenden
    time1 = time1/CLOCKS_PER_SEC; //Auf Sekunden skalieren
    cout << "Time: " << time1 << "s" << endl;

    //cout << "test" << endl;
    return 0;
}