//
// Created by caspar on 12.08.18.
//

#ifndef MESSTOOL_MEASURESIGNALSTRENGTH_H
#define MESSTOOL_MEASURESIGNALSTRENGTH_H

#include <cstring>
#include <string>
#include <iterator>
#include <regex>

using namespace std;

void start_measureSignalStrength();

void extractRelevantSignalParameter(string txtfile);

#endif //MESSTOOL_MEASURESIGNALSTRENGTH_H
