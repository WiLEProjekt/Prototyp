#ifndef PACKETLOSSAUSWERTUNG_SAGE_H
#define PACKETLOSSAUSWERTUNG_SAGE_H

#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <stdio.h>

using namespace std;

/**
 * Reads the temprary sage-matrix-file, builds the matrices and deletes the temporary file
 * @param transitionmatrix
 * @param emissionmatrix
 * @param initialprobabilities
 */
void readMatrixFile(vector <vector<double>> &transitionmatrix, vector <vector<double>> &emissionmatrix, vector<double> &initialprobabilities);

#endif //PACKETLOSSAUSWERTUNG_SAGE_H
