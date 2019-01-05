#ifndef PACKETLOSSAUSWERTUNG_GENERATOR_H
#define PACKETLOSSAUSWERTUNG_GENERATOR_H

#include <iostream>
#include <string>
#include <vector>
#include <random>

using namespace std;

void setSeed(int seed);
float generateRandomNumber();
vector<int> buildGilbertElliot(long numPackets, float p, float r, float k, float h);
vector<int> buildMarkov(long numPackets, float p13, float p31, float p32, float p23, float p14, float p41=1.0);
void calculateBursts(vector<bool> &trace, int &i, int &temp, vector<int> &genSizes);


#endif //PACKETLOSSAUSWERTUNG_GENERATOR_H
