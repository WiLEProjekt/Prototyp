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
float generateRandomNumber();

/**
 * Generates a Trace using the GilbertElliot Model.
 * @param numPackets
 * @param p
 * @param r
 * @param k
 * @param h
 * @return
 */
vector<int> buildGilbertElliot(long numPackets, float p, float r, float k, float h);

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
vector<int> buildMarkov(long numPackets, float p13, float p31, float p32, float p23, float p14, float p41=1.0);

/**
 * Calculates the burstsizes, of which the ECDFs are later generated
 * @param trace
 * @param i
 * @param temp
 * @param genSizes
 */
void calculateBursts(vector<bool> &trace, int &i, int &temp, vector<int> &genSizes);


#endif //PACKETLOSSAUSWERTUNG_GENERATOR_H
