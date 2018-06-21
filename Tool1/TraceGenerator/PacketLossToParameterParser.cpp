#include <fstream>
#include <iostream>
#include "PacketLossToParameterParser.h"
#include "HMM.h"
#include <algorithm>

PacketLossToParameterParser::PacketLossToParameterParser(PacketLossModel packetLossModel, string filename) {
    this->filenname = filename;
    this->packetLossModel = packetLossModel;
}

float *PacketLossToParameterParser::parseParameter() {
    vector<bool> trace = this->readFile(this->filenname);
    switch (packetLossModel) {
        case BERNOULI:
            return this->parseBernoulli(trace);
        case SIMPLE_GILBERT:
            return this->parseSimpleGilbert(trace);
        case GILBERT:
            return this->parseGilbert(trace);
        case GILBERT_ELLIOT:
            return this->parseGilbertElliot(trace);
        case MARKOV:
            return this->parseMarkov(trace);
    }
    return nullptr;
}

float *PacketLossToParameterParser::parseGilbert(vector<bool> trace) {
    float a, b, c = 0;
    float r, h, p;
    unsigned long lossCount = 0;
    unsigned long lossAfterLossCount = 0;
    unsigned long threeLossesCount = 0;
    unsigned long lossRecieveLossCount = 0;
    vector<unsigned long> burstLenghtes;
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
                burstLenghtes.push_back(currentBurstLength);
                currentBurstLength = 0;
            }
        }
    }

    unsigned long burstSum = 0;
    for (unsigned long burst : burstLenghtes) {
        burstSum += burst;
    }

    float avrgBurstLength = (float) burstSum / (float) burstLenghtes.size();

    a = 1.f / (float) trace.size() * (float) lossCount;
    b = 1.f / lossCount * (float) lossAfterLossCount;
    c = 1.f / (lossRecieveLossCount + threeLossesCount) * threeLossesCount;

    r = 1 - (a * c - b * b) / (2 * a * c - (b * (a + c)));

    //r = 1.f / avrgBurstLength;

    h = 1.f - (b / (1.f - r));
    p = (a * r) / (1.f - h - a);

    cout << "a: " << a << endl << "b: " << b << endl << "c: " << c << endl << "r: " << r << endl << "h: " << h << endl
         << "p: " << p << endl << "lossAfterLossCount: " << lossAfterLossCount << endl << "threeLossesCount: "
         << threeLossesCount << endl << "lossRecieveLossCount: " << lossRecieveLossCount << endl
         << "lossCount: " << lossCount << endl << "avrgBurstSize: " << avrgBurstLength << endl << "BurstSum: "
         << burstSum << endl << "BurstsCount: " << burstLenghtes.size() << endl;

    return new float[4]{p, r, 1, h};
}

vector<bool> PacketLossToParameterParser::readFile(string filename) {
    vector<bool> trace;
    ifstream file;
    file.open(filename, ios::in);
    char c;
    while (file.get(c)) {
        trace.push_back(c != '0');
    }
    return trace;
}

float *PacketLossToParameterParser::parseMarkov(vector<bool> trace) {
    //Zustände
    int loss = 0;
    int recieve = 1;
    int pi_min[2];
    int pi_max[2];
    pi_min[loss] = 1;
    pi_max[loss] = 1000;
    pi_min[recieve] = 0;
    pi_min[recieve] = 0;

    double a[4][4];
    a[0][0] = 0;
    a[0][1] = 0;
    a[0][2] = 0;
    a[0][3] = 0;
    a[1][0] = 0;
    a[1][1] = 0;
    a[1][2] = 0;
    a[1][3] = 0;
    a[2][0] = 0;
    a[2][1] = 0;
    a[2][2] = 0;
    a[2][3] = 0;
    int p_14 = 0;
    int p_41 = 0;
    int p_13 = 0;
    int p_31 = 0;
    int p_32 = 0;
    int p_23 = 0;
    int random_number = (int) (rand() * 1000) + 1;
    int state;

    for (int i = 0; i < trace.size(); ++i) {

    }

    HMM *hmm = new HMM();
    hmm->train(trace, trace.size());
    hmm->print();
    return nullptr;
}

float *PacketLossToParameterParser::parseSimpleGilbert(vector<bool> trace) {
    unsigned long lossCounter = 0;
    unsigned long recieveCounter = 0;
    unsigned long lossAfterRecieveCounter = 0;
    unsigned long recieveAfterLossCounter = 0;

    for (int i = 0; i < trace.size(); i++) {
        if (trace[i]) {
            recieveCounter++;
            if (i != 0) {
                if (!trace[i - 1]) {
                    recieveAfterLossCounter++;
                }
            }
        } else {
            lossCounter++;
            if (i != 0) {
                if (trace[i - 1]) {
                    lossAfterRecieveCounter++;
                }
            }
        }
    }

    float p = 1.f / (float) recieveCounter * (float) lossAfterRecieveCounter;
    float r = 1.f / (float) lossCounter * (float) recieveAfterLossCounter;

    return new float[4]{p, r, 1, 0};
}

float *PacketLossToParameterParser::parseGilbertElliot(vector<bool> trace) {
    vector<int> lossindices;
    vector<int> gapindices;
    for(int i = 0; i<trace.size(); i++){ //find all loss indices
        if(!trace[i]){
            cout << i << endl;
            lossindices.push_back(i);
        }
    }

    for(int i = 1; i<lossindices.size(); i++){ //push
        if(lossindices[i]-lossindices[i-1]-1 < 16){
            if(!(find(gapindices.begin(), gapindices.end(), lossindices[i-1])!=gapindices.end())){
                gapindices.push_back(lossindices[i-1]);
            }
            if(!(find(gapindices.begin(), gapindices.end(), lossindices[i])!=gapindices.end())){
                gapindices.push_back(lossindices[i]);
            }
        }
    }

    while()
    cout << "gapindices" << endl;
    for(int i = 0; i<gapindices.size();i++){
        cout << gapindices[i] << endl;
    }
    return nullptr;
}

float *PacketLossToParameterParser::parseBernoulli(vector<bool> trace) {
    unsigned long lossCounter = 0;
    for (auto &&i : trace) {
        if (!i) {
            lossCounter++;
        }
    }

    float p = 1.f / (float) trace.size() * (float) lossCounter;

    return new float[4]{p, 1 - p, 1, 0};
}