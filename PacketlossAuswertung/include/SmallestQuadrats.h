#ifndef PACKETLOSSAUSWERTUNG_SMALLESTQUADRATS_H
#define PACKETLOSSAUSWERTUNG_SMALLESTQUADRATS_H
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cfloat>

using namespace std;
#define MAXSEED 100
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
 * Calculates the Squared difference of two values and returns the result
 * @param value1
 * @param value2
 * @return
 */
double calcSquaredDifference(float value1, float value2);

/**
 * Calculates the Kolmogorov-distance of two values and returns the result
 * @param value1
 * @param value2
 * @return
 */
double calcKolmogorovDistance(float value1, float value2);

/**
 * Calculates the Kolmogorov and Suared distance of two given ECDFs
 * @param ECDF1
 * @param ECDF2
 * @param kolmogorovDistance
 * @param squaredDifference
 */
void calcDistance(vector<vector<float> > &ECDF1, vector<vector<float> > &ECDF2, double &kolmogorovDistance, double &squaredDifference);

/**
 * Finds best Seed and smallest Squared Difference given Modelparameters for Gilbert Elliot Model
 * @param length
 * @param origECDF
 * @param p
 * @param r
 * @param k
 * @param h
 */
void fitGilbert(long length, vector<vector<float> > origECDF, double p, double r, double k, double h, vector<int> &returnBurstsizesKolmogorov, int &seedKolmogorov, double &kolmogorovDistance, vector<int> &returnBurstsizesLeastSquared, int &seedLeastSquared, double &LeastSquaredDifference);

/**
 * Finds best Seed and smallest Squared Difference given Modelparameters for Markov Model
 * @param length
 * @param origECDF
 * @param p13
 * @param p31
 * @param p32
 * @param p23
 * @param p14
 */
void fitMarkov(long length, vector<vector<float> > origECDF, double p13, double p31, double p32, double p23, double p14, vector<int> &returnBurstsizesKolmogorov, int &seedKolmogorov, double &kolmogorovDistance, vector<int> &returnBurstsizesLeastSquared, int &seedLeastSquared, double &LeastSquaredDifference);


#endif //PACKETLOSSAUSWERTUNG_SMALLESTQUADRATS_H
