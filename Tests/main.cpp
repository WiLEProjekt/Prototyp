#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;
mt19937 generator; //Mersenne Twister Engine
uniform_real_distribution<float> distribution;

float genRandomNumber(){
    return(distribution(generator));
}

vector<bool> readFile(string filename){
    vector<bool> trace;
    ifstream file;
    file.open(filename, ios::in);
    char c;
    while (file.get(c)) {
        trace.push_back(c != '0');
    }
    return trace;
}

vector<bool> createGilbertElliotTrace(int packetnumber, float p, float r, float k, float h, float& lost){
    vector<bool> trace;
    bool good = true; // 1 = good state, 0 = bad state
    bool send = true; // 0 = loss, 1 = successfully send, or no loss
    int loss = 0;
    for(long i = 0; i< packetnumber+1; i++){
        if(good){ //in good state
            float randomNumberLossK = genRandomNumber(); //Random Number that indicates if a packet is lost while being in a state
            float randomNumberStateP = genRandomNumber(); //Random number that indicates a state transition from good -> bad
            send = !(randomNumberLossK <= (1.0-k)); //Calculate next state
            good = !(randomNumberStateP <= p); //Calculate next state
            // cout << "random k: " << randomNumberLossK << " k: " << this->k << " random p: " << randomNumberStateP << " p: " << this->p << endl;

        }else{ //in bad state
            float randomNumberLossH = genRandomNumber(); //Random Number that indicates if a packet is lost while being in a state
            float randomNumberStateR = genRandomNumber(); //Random number that indicates a state transition from bad -> good
            send = !(randomNumberLossH <= (1.0-h));
            good = (randomNumberStateR <= r); //Calculate next state
            //cout << "random h: " << randomNumberLossH << " h: " << this->h << " random r: " << randomNumberStateR << " r: " << this->r << endl;
        }
        if(i != 0) {
            trace.push_back(send);
            if(!send){
                loss++;
            }
        }
    }
    lost = (float)loss/trace.size()*100;
    return trace;
}

void calcLoss(vector<bool>& trace, float& result){
    int loss=0;
    for(int i = 0; i<trace.size(); i++){
        if(!trace[i]) {
            loss++;
        }
    }
    result = (float)loss/trace.size()*100;
}

int main(int argc, char **argv) {
    generator.seed(1);
    uniform_real_distribution<float> *dist = new uniform_real_distribution<float>(0.0, 1.0);
    distribution = *dist;
    if(argc != 2){
        cout << "usage: ./main filename" << endl;
    }else{
        string filename = argv[1];
        vector<bool> origTrace = readFile(filename);
        vector<bool> testTrace;
        float origLoss;
        float testLoss;
        calcLoss(origTrace, origLoss); //Calculate lossrate of the original Trace

        //Iterate over all parameter possibilities
        for(int p = 0; p <101; p++){
            for(int r = 0; r<101; r++){
                for(int k = 50; k<101; k++){
                    for(int h = 0; h<50; h++){
                        float pf = (float)p/100;
                        float rf = (float)r/100;
                        float kf = (float)k/100;
                        float hf = (float)h/100;
                        testTrace.clear();
                        testTrace = createGilbertElliotTrace(200000, pf, rf, kf, hf, testLoss); //Generate new test
                        if(fabs(testLoss-origLoss)<0.5){
                            //Burstlengths zählen
                            //Mach den Kolmogorov-Smirnov Test
                            //Wenn Test erfolgreich,
                        }
                    }
                }
            }
        }
    }


    return 0;
}