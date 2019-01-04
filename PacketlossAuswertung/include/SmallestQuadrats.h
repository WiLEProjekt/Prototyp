#ifndef PACKETLOSSAUSWERTUNG_SMALLESTQUADRATS_H
#define PACKETLOSSAUSWERTUNG_SMALLESTQUADRATS_H
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cfloat>

using namespace std;
#define MAXSEED 1
/**
 * Calculates the ECDF
 * @param sizes - vector with the burstsizes of which the ECDF should be calculated. Equals @Param overallsizes of method calcLoss
 * @param ECDF - Vector representing the ECDF. [][] first dimension represents the burst size (x-axis), second the cumulative-probability (y-axis)
 */
void calculateECDF(vector<int> &sizes, vector<vector<float> > &ECDF);

/**
 * Calculates the burstsizes
 * @param trace - trace of which the burst sizes should be calculated
 * @param overallsizes - vector with the results
 */
void calcLoss(vector<bool> &trace, vector<int> &overallsizes);

/**
 * Brings both ECDFs on the same length by adding elements at the beginning and at the end, so they can be compared better
 * @param ECDF1
 * @param ECDF2
 */
void adjustECDFLengths(vector<vector<float> > &ECDF1, vector<vector<float> > &ECDF2);

/**
 * Calculates the Squared difference of two ECDFs and returns the result
 * @param ECDF1
 * @param ECDF2
 * @return
 */
double calcSquaredDifference(vector<vector<float> > &ECDF1, vector<vector<float> > &ECDF2);

int fitGilbert(long length, vector<vector<float> > origECDF, float p, float r, float k, float h);

int fitMarkov(long length, vector<vector<float> > origECDF, float p13, float p31, float p32, float p23, float p14);


#endif //PACKETLOSSAUSWERTUNG_SMALLESTQUADRATS_H
