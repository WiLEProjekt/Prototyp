#include <fstream>
#include <iostream>
#include "PacketLossToParameterParser.h"
#include <algorithm>
#include <map>

PacketLossToParameterParser::PacketLossToParameterParser(PacketLossModelType packetLossModel, string filename) {
    this->filename = filename;
    this->packetLossModel = packetLossModel;
}

ExtractParameter PacketLossToParameterParser::parseParameter(unsigned int gMin) {
    vector<bool> trace = this->readFile(this->filename);
    float *parameter;
    switch (packetLossModel) {
        case BERNOULI:
            parameter = this->parseBernoulli(trace);
            break;
        case SIMPLE_GILBERT:
            parameter = this->parseSimpleGilbert(trace);
            break;
        case GILBERT:
            parameter = this->parseGilbert(trace);
            break;
        case GILBERT_ELLIOT:
            parameter = this->parseGilbertElliot(trace, gMin);
            break;
        case MARKOV:
            parameter = this->parseMarkov(trace, gMin);
            break;
    }
    ExtractParameter extractParameter{};
    extractParameter.parameter = parameter;
    extractParameter.packetCount = trace.size();
    return extractParameter;
}

ExtractParameter PacketLossToParameterParser::parseParameter() {
    this->parseParameter(G_MIN_DEFAULT);
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

float *PacketLossToParameterParser::parseMarkov(vector<bool> trace, unsigned int gMin) {
    const unsigned int G_MIN = 4;
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
            if (i > G_MIN) {
                gapPeriod = true;
                for (int j = 0; j < G_MIN; j++) {
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

float *PacketLossToParameterParser::parseGilbertElliot(vector<bool> trace, unsigned int gMin) {
    vector<int> lossindices;
    vector<int> gapindices;
    for (int i = 0; i < trace.size(); i++) { //find all loss indices
        if (!trace[i]) {
            cout << i << endl;
            lossindices.push_back(i);
        }
    }

    for (int i = 1; i < lossindices.size(); i++) { //push
        if (lossindices[i] - lossindices[i - 1] - 1 < 16) {
            if (!(find(gapindices.begin(), gapindices.end(), lossindices[i - 1]) != gapindices.end())) {
                gapindices.push_back(lossindices[i - 1]);
            }
            if (!(find(gapindices.begin(), gapindices.end(), lossindices[i]) != gapindices.end())) {
                gapindices.push_back(lossindices[i]);
            }
        }
    }

    //while()
    cout << "gapindices" << endl;
    for (int i = 0; i < gapindices.size(); i++) {
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