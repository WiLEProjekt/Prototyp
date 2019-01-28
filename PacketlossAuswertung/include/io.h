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

/**
 * Writes the calculated model-parameter to a file for further analysation
 * @param output
 * @param pBernoulli
 * @param rBernoulli
 * @param pSimpleGilbert
 * @param rSimpleGilbert
 * @param pGilbert
 * @param rGilbert
 * @param kGilbert
 * @param hGilbert
 * @param pGilbertElliot
 * @param rGilbertElliot
 * @param kGilbertElliot
 * @param hGilbertElliot
 * @param p13
 * @param p31
 * @param p32
 * @param p23
 * @param p14
 * @param p41
 */
void writeParams(string output, double pBernoulli, double rBernoulli, double pSimpleGilbert, double rSimpleGilbert, double pGilbert, double rGilbert, double kGilbert, double hGilbert, double pGilbertElliot, double rGilbertElliot, double kGilbertElliot, double hGilbertElliot, double p13, double p31, double p32, double p23, double p14, double p41);

/**
 * writes the csv header
 * @param output
 */
void writeHeader(string output);

/**
 * writes the csv data under csv header
 * @param output
 * @param model
 * @param metric
 * @param bestSeed
 * @param bestDistance
 * @param confInterval
 */
void consolePrint(string output, string model, string metric, int bestSeed, double bestDistance, vector<double> &confInterval);

/**
 * writes the STates header
 * @param output
 */
void writeStatesHeader(string output);
/**
 * Writes the states to a file
 * @param output
 * @param one states
 * @param two
 * @param three
 * @param four
 */
void writeStates(string output, unsigned long long one, unsigned long long two, unsigned long long three=0, unsigned long long four=0);

#endif //PACKETLOSSAUSWERTUNG_IO_H
