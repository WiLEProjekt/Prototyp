//
// Created by drieke on 09.08.18.
//

#ifndef MESSTOOL_MEASUREBANDWIDTH_H
#define MESSTOOL_MEASUREBANDWIDTH_H

#include <cstdlib> /* to start netperf in console */

#include <sstream> /* split lines */
#include <vector> /* substrings of line */
#include <fstream> /* reading & writing files */
#include <iostream> /* reading & writing files */
#include <regex> /* check is value is double */
#include <cstring>
#include <string.h>

using namespace std;

bool is_positiv_number(string str);

void measureThroughput(const char* filename);


#endif //MESSTOOL_MEASUREBANDWIDTH_H
