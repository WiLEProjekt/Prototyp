#ifndef PACKETLOSSAUSWERTUNG_GENERATOR_H
#define PACKETLOSSAUSWERTUNG_GENERATOR_H

#include <iostream>
#include <string>
#include <vector>
#include <random>

using namespace std;

/**
 * Sets the seed of the random number generator
 * @param seed
 */
void setSeed(int seed);

/**
 * Generates a random number
 * @return
 */
double generateRandomNumber();

/**
 * Generates a Trace using the GilbertElliot Model.
 * @param numPackets
 * @param p
 * @param r
 * @param k
 * @param h
 * @return
 */
vector<int> buildGilbertElliot(long numPackets, double p, double r, double k, double h);

/**
 * Generates a Trace using the Markov Model.
 * @param numPackets
 * @param p13
 * @param p31
 * @param p32
 * @param p23
 * @param p14
 * @param p41
 * @return
 */
vector<int> buildMarkov(long numPackets, double p13, double p31, double p32, double p23, double p14, double p41=1.0);

/**
 * Calculates the burstsizes, of which the ECDFs are later generated
 * @param trace
 * @param i
 * @param temp
 * @param genSizes
 */
void calculateBursts(vector<bool> &trace, int &i, int &temp, vector<int> &genSizes);


#endif //PACKETLOSSAUSWERTUNG_GENERATOR_H
