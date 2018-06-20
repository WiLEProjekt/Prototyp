//
// Created by drieke on 20.06.18.
//

#ifndef TRACEGENERATOR_HMM_H
#define TRACEGENERATOR_HMM_H

#include <vector>
#include <iostream>

using namespace std;

class HMM {
private:
    int numStates;
    int sigmaSize = 1;
    vector<double> pi; // Anfangswahrscheinlichkeiten
    vector<vector<double>> a; //Übergangswahrscheinlichkeiten
    vector<vector<double>> b; //Ausgabewahrscheinlichkeiten

public:
    HMM() {
        numStates = 4;
    }

    void train(vector<bool> o, unsigned long steps);

    vector<vector<double> > backwardProc(vector<bool> o);

    vector<vector<double> > forwardProc(vector<bool> o);

    double p(unsigned long t, unsigned long i, unsigned long j, vector<bool> o, vector<vector<double> > fwd,
             vector<vector<double> > bwd);

    double
    gamma(unsigned long i, unsigned long t, vector<bool> o, vector<vector<double> > fwd, vector<vector<double> > bwd);

    double divide(double n, double d);

    void print();

    void initArray(vector<vector<double> > &v, unsigned long width, unsigned long height);

    void initArraySmall(vector<double> &v, unsigned long size);

    void initArray(vector<vector<double>> &v, unsigned long size);
};


#endif //TRACEGENERATOR_HMM_H
