#ifndef PACKETLOSSAUSWERTUNG_IO_H
#define PACKETLOSSAUSWERTUNG_IO_H
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

/**
 * Reads the Trace and converts the string character-wise into bools
 * @param filename - the path to the Inputfile
 * @return
 */
vector<bool> readBinaryTrace(string filename);

/**
 * Writes the burstsizes to a file, which then can be easily plotted
 * @param output - outputpath
 * @param bursts - vector containing the burstsizes
 */
void writeVector(string output, vector<int> &outvec);

void writeVector(string output, vector<double> &outvec);

#endif //PACKETLOSSAUSWERTUNG_IO_H
