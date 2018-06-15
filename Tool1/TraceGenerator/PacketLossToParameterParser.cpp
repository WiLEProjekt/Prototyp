//
// Created by drieke on 15.06.18.
//

#include <fstream>
#include <iostream>
#include "PacketLossToParameterParser.h"

PacketLossToParameterParser::PacketLossToParameterParser(PacketLossModel packetLossModel, string filename) {
    this->filenname = filename;
    this->packetLossModel = packetLossModel;
}

float *PacketLossToParameterParser::parseParameter() {
    vector<bool> trace = this->readFile(this->filenname);
    switch (packetLossModel) {
        case BERNOULI:
            return this->parseBernouli(trace);
        case SIMPLE_GILBERT:
            return this->parseSimpleGilber(trace);
        case GILBERT:
            return this->parseGilber(trace);
        case GILBERT_ELLIOT:
            return this->parseGilbertElliot(trace);
        case MARKOV:
            return this->parseMarkov(trace);
    }
    return nullptr;
}

float *PacketLossToParameterParser::parseGilber(vector<bool> trace) {
    float a, b, c = 0;
    float r, h, p;
    unsigned long lossCount = 0;
    unsigned long lossAfterLossCount = 0;
    unsigned long threeLossesCount = 0;
    unsigned long lossRecieveLossCount = 0;

    for (unsigned long i = 0; i < trace.size(); i++) {
        if (!trace[i]) {
            lossCount++;

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
        }
    }

    a = 1.f / (float) trace.size() * (float) lossCount;
    b = 1.f / lossCount * (float) lossAfterLossCount;
    float threeLossesChance = 1.f / (float) trace.size() * (float) threeLossesCount;
    float lossRecieveLossChance = 1.f / (float) trace.size() * (float) lossRecieveLossCount;

    if (threeLossesChance != 0) {
        c = threeLossesChance / (lossRecieveLossChance + threeLossesChance);
    }


    r = 1 - ((a * c - b * b) / (2.f * a * c - b * (a + c)));
    h = 1.f - (b / (1.f - r));
    cout << "(" << a << " * " << r << ") / (1 - " << h << " - " << a << ")" << endl;
    cout << "(" << a * r << ") / (" << 1 - h - a << ")" << endl;
    p = (a * r) / (1.f - h - a);

    cout << "a: " << a << endl << "b: " << b << endl << "c: " << c << endl << "r: " << r << endl << "h: " << h << endl
         << "p: " << p << endl << "lossAfterLossCount: " << lossAfterLossCount << endl << "threeLossesCount: "
         << threeLossesCount << endl << "lossRecieveLossCount: " << lossRecieveLossCount << endl;

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
    return nullptr;
}

float *PacketLossToParameterParser::parseSimpleGilber(vector<bool> trace) {
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

    float p = 1.f / (float) lossCounter * (float) recieveAfterLossCounter;
    float r = 1.f / (float) recieveCounter * (float) lossAfterRecieveCounter;

    return new float[4]{p, r, 1, 0};
}

float *PacketLossToParameterParser::parseGilbertElliot(vector<bool> trace) {
    return nullptr;
}

float *PacketLossToParameterParser::parseBernouli(vector<bool> trace) {
    unsigned long lossCounter = 0;
    for (int i = 0; i < trace.size(); i++) {
        if (!trace[i]) {
            lossCounter++;
        }
    }

    float p = 1.f / (float) trace.size() * (float) lossCounter;

    return new float[4]{p, 1 - p, 1, 0};
}
