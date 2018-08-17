//
// Created by caspar on 12.08.18.
//

#ifndef MESSTOOL_PACKETPAIRBANDWIDTH_H
#define MESSTOOL_PACKETPAIRBANDWIDTH_H

#include <vector>
#include <fstream>

using namespace std;

int calculateAvgBandwidth(vector<int> origbandwidths);

int calculateMedian(vector<int> bandwidths);

void writeBandwidths(vector<int> bandwidts);

uint64_t getTimens();

void wait(int msec);

uint64_t getTimens();

#endif //MESSTOOL_PACKETPAIRBANDWIDTH_H
