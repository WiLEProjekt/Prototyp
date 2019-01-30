#include <Generator.h>

mt19937 randomGenerator; //Mersenne Twister Engine
uniform_real_distribution<double> distribution;

void setSeed(int seed) {
    randomGenerator.seed(seed);
}

double generateRandomNumber() {
    return (distribution(randomGenerator));
}

void calculateBursts(vector<bool> &trace, int &i, int &temp, vector<int> &genSizes) {
    if (i > 1) {
        if (!trace[i - 1] && !trace[i - 2]) {
            temp--;
        } else if (trace[i - 1] && trace[i - 2]) {
            temp++;
        } else {
            genSizes.push_back(temp);
            temp = 0;
            if (!trace[i - 1]) {
                temp--;
            } else if (trace[i - 1]) {
                temp++;
            }
        }
    } else if (i == 1) {
        if (!trace[i - 1]) {
            temp--;
        } else if (trace[i - 1]) {
            temp++;
        }
    }
}

vector<int> buildGilbertElliot(long numPackets, double p, double r, double k, double h, unsigned long long &goods,
                               unsigned long long &bads) {
    vector<bool> trace;
    vector<int> gensizes;
    bool good = true; // 1 = good state, 0 = bad state
    bool send; // 0 = loss, 1 = successfully send, or no loss
    int temp = 0;
    bool prev;
    for (int i = 0; i < numPackets + 1; i++) {
        //generate Trace
        if (good) { //in good state
            prev = true;
            double randomNumberLossK = generateRandomNumber(); //Random Number that indicates if a packet is lost while being in a state
            double randomNumberStateP = generateRandomNumber(); //Random number that indicates a state transition from good -> bad
            send = !(randomNumberLossK <= (1.0 - k)); //Calculate next state
            good = !(randomNumberStateP <= p); //Calculate next state
        } else { //in bad state
            prev = false;
            double randomNumberLossH = generateRandomNumber(); //Random Number that indicates if a packet is lost while being in a state
            double randomNumberStateR = generateRandomNumber(); //Random number that indicates a state transition from bad -> good
            send = !(randomNumberLossH <= (1.0 - h));
            good = (randomNumberStateR <= r); //Calculate next state
        }

        if (i != 0) {
            trace.push_back(send);
            if (prev) {
                goods++;
            } else if (!prev) {
                bads++;
            }
        }
        //calculate burstsizes
        calculateBursts(trace, i, temp, gensizes);
    }
    gensizes.push_back(temp);
    return (gensizes);
}

vector<int>
buildMarkov(long numPackets, double p13, double p31, double p32, double p23, double p14, unsigned long long &stateOne,
            unsigned long long &stateTwo, unsigned long long &stateThree, unsigned long long &stateFour) {
    double p41 = 1.0;
    vector<bool> trace;
    vector<int> gensizes;
    int temp = 0;
    int state = 1;
    int prevstate = 0;

    for (int i = 0; i < numPackets + 1; i++) {
        double randomValue = generateRandomNumber();
        bool value;
        switch (state) {
            case 1:
                prevstate = 1;
                if (randomValue <= p13) {
                    state = 3;
                } else if (randomValue <= p13 + p14) {
                    state = 4;
                }
                value = true;
                break;
            case 2:
                prevstate = 2;
                if (randomValue <= p23) {
                    state = 3;
                }
                value = true;
                break;
            case 3:
                prevstate = 3;
                if (randomValue <= p32) {
                    state = 2;
                } else if (randomValue <= p32 + p31) {
                    state = 1;
                }
                value = false;
                break;
            case 4:
                prevstate = 4;
                if (randomValue <= p41) {
                    state = 1;
                }
                value = false;
                break;
        }
        if (i != 0) {
            trace.push_back(value);
            if (prevstate == 1) {
                stateOne++;
            } else if (prevstate == 2) {
                stateTwo++;
            } else if (prevstate == 3) {
                stateThree++;
            } else if (prevstate == 4) {
                stateFour++;
            }
        }

        //calculate burstsizes
        calculateBursts(trace, i, temp, gensizes);
    }
    gensizes.push_back(temp);
    return (gensizes);
}