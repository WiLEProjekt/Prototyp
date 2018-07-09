#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <math.h>
#include <time.h>

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

void createGilbertElliotTrace(int packetnumber, float p, float r, float k, float h, vector<vector<float> > &avgburstszs) {
    vector<bool> trace;
    vector<int> burstsizes;
    vector<int> goodsizes;
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
                }else{
                    goodsizes.push_back(temp);
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
}

void calcLoss(vector<bool> &trace, float &result, float &burstsize, float &goodsize) {
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
                }else{
                    goodsizes.push_back(temp);
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

int main(int argc, char **argv) {
    double time1=0.0, tstart1;
    tstart1 = clock(); //Zeitmessung starten
    generator.seed(1);
    uniform_real_distribution<float> *dist = new uniform_real_distribution<float>(0.0, 1.0);
    distribution = *dist;
    if (argc != 2) {
        cout << "usage: ./main filename" << endl;
    } else {
        string filename = argv[1];
        vector<bool> origTrace = readFile(filename);
        float origLoss, origburstsize, origgoodsize;
        calcLoss(origTrace, origLoss, origburstsize, origgoodsize); //Calculate lossrate of the original Trace
        vector<vector<float> > possibleParams;
        vector<vector<float> > avgburstsizes;
        for (int p = 1; p < 51; p++) {
            for (int r = 50; r < 101; r++) {
                for (int h = 1; h < 51; h++) {
                    float pf = (float) p / 100;
                    float rf = (float) r / 100;
                    //float kf = (float) k / 100;
                    float hf = (float) h / 100;
                    float theoreticalLoss = (1-hf)*(pf/(pf+rf))*100;
                    if(fabs(theoreticalLoss-origLoss)<0.1){
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
        //cout << "paramsize: " << possibleParams.size() << endl;
        for(int i = 0; i<possibleParams.size(); i++) {
            createGilbertElliotTrace(20000, possibleParams[i][0], possibleParams[i][1], 1.0, possibleParams[i][2], avgburstsizes);
        }


        float lowestdiff = 1000;
        int lowestindex=0;
        for(int i = 0; i<avgburstsizes.size(); i++){
            if((fabs(avgburstsizes[i][1]-origburstsize) + fabs(avgburstsizes[i][2]-origgoodsize))<lowestdiff){
                lowestdiff=fabs(avgburstsizes[i][1]-origburstsize)+ fabs(avgburstsizes[i][2]-origgoodsize);
                lowestindex=i;
            }
        }
        cout << "found parameters: p: " << possibleParams[lowestindex][0] << " r: " << possibleParams[lowestindex][1] << " h: " << possibleParams[lowestindex][2] << endl;
        cout << "original Lossrate: " << origLoss << " original avg Burstsize: " << origburstsize << " original good size: " << origgoodsize << endl;
        cout << "found Lossrate: " << avgburstsizes[lowestindex][0] << " found avg Burstsize: " << avgburstsizes[lowestindex][1] << endl;
    }
    time1 = clock()-tstart1; //Zeitmessung beenden
    time1 = time1/CLOCKS_PER_SEC; //Auf Sekunden skalieren
    cout << "Time: " << time1 << "s" << endl;

    //cout << "test" << endl;
    return 0;
}