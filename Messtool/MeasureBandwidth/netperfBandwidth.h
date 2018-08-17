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

/**
 * Simple function to measure the throughput based on netperf. Netperf is started and the output is logged.
* The troughput is written to a outputfile 'filename'.
 * @param filename the file containing the throughput
 * @param netPerf the terminal call of netperf containg the parameters
 * @return -1 if some error occured
 */

int measureThroughputViaUDP_Stream(const char *filename, string netperf);


#endif //MESSTOOL_MEASUREBANDWIDTH_H
