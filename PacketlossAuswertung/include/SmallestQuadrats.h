#ifndef PACKETLOSSAUSWERTUNG_SMALLESTQUADRATS_H
#define PACKETLOSSAUSWERTUNG_SMALLESTQUADRATS_H
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Calculates the ECDF
 * @param sizes
 * @param distFunction
 */
void calculateECDF(vector<int> &sizes, vector<vector<float> > &ECDF);

/**
 * Calculates the size of lossbursts and received bursts
 * @param trace
 * @param lossrate
 * @param burstsize
 * @param goodsize
 * @param overallsizes
 */
void calcLoss(vector<bool> &trace, vector<int> &overallsizes);


#endif //PACKETLOSSAUSWERTUNG_SMALLESTQUADRATS_H
