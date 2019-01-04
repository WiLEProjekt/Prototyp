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

#endif //PACKETLOSSAUSWERTUNG_IO_H
