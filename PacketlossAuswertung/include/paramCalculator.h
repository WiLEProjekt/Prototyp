#ifndef PACKETLOSSAUSWERTUNG_PARAMCALCULATOR_H
#define PACKETLOSSAUSWERTUNG_PARAMCALCULATOR_H
#include <iostream>
#include <vector>
using namespace std;

/**
 * calculates the Parameter for the Bernoulli-Model of a given trace
 * @param trace
 */
void getBernoulliParams(vector<bool> &trace, double &p);

/**
 * calculates the Parameter for the SimpleGilbert-Model of a given trace
 * @param trace
 */
void getSimpleGilbertParams(vector<bool> &trace, double &p, double &r);

#endif //PACKETLOSSAUSWERTUNG_PARAMCALCULATOR_H
