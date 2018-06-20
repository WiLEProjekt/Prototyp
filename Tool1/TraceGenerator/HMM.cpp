//
// Created by drieke on 20.06.18.
//

#include "HMM.h"

#include <vector>

void HMM::initArraySmall(vector<double> &v, unsigned long size) {
    for (int i = 0; i < size; i++) {
        v.push_back(0);
    }
}

void HMM::initArray(vector<vector<double> > &v, unsigned long width, unsigned long height) {
    for (int i = 0; i < width; i++) {
        vector<double> innerVector;
        v.push_back(innerVector);
        initArraySmall(v[i], height);
    }
}

void HMM::initArray(vector<vector<double> > &v, unsigned long size) {
    for (int i = 0; i < size; i++) {
        vector<double> innerVector;
        v.push_back(innerVector);
        initArraySmall(v[i], size);
    }
}

void HMM::train(vector<bool> o, unsigned long steps) {
    unsigned long T = o.size();
    vector<vector<double> > fwd;
    vector<vector<double> > bwd;
    vector<double> pi1;
    initArraySmall(pi1, numStates);
    vector<vector<double>> a1;
    initArray(a1, numStates);
    vector<vector<double>> b1;
    initArray(b1, numStates);
    for (int s = 0; s < steps; s++) {
/* Berechnen der Forward- und Backward Variablen bzgl. des
aktuellen Modells */
        fwd = forwardProc(o);
        bwd = backwardProc(o);
/* Neusch ̈atzung der Anfangswahrscheinlichkeiten */
        for (int i = 0; i < numStates; i++)
            pi1[i] = gamma(i, 0, o, fwd, bwd);
/* Neusch ̈atzung der
 ̈
Ubergangswahrscheinlichkeiten */
        for (int i = 0; i < numStates; i++) {
            for (int j = 0; j < numStates; j++) {
                double num = 0;
                double denom = 0;
                for (int t = 0; t <= T - 1; t++) {
                    num += p(t, i, j, o, fwd, bwd);
                    denom += gamma(i, t, o, fwd, bwd);
                }
                a1[i][j] = divide(num, denom);
            }
        }
/* Neusch ̈atzen der Ausgabewahrscheinlichkeiten */
        for (int i = 0; i < numStates; i++) {
            for (int k = 0; k < sigmaSize; k++) {
                double num = 0;
                double denom = 0;
                for (int t = 0; t <= T - 1; t++) {
                    double g = gamma(i, t, o, fwd, bwd);
                    num += g * (k == o[t] ? 1 : 0);
                    denom += g;
                }
                b1[i][k] = divide(num, denom);
            }
        }
        pi = pi1;
        a = a1;
        b = b1;
    }
}

/** Berechnet die Backward-Variablen b(i,t) f ̈ur Zust ̈ande i zum Zeitpunkt
t unter einer Realisation O und der aktuellen Parameterbelegung
@param o die Realisation O
@return ein Verbund b(i,t)  ̈uber die Zust ̈ande und die Zeit, der die
Backward-Variablen enth ̈alt.
*/
vector<vector<double> > HMM::backwardProc(vector<bool> o) {
    unsigned long T = o.size();
    vector<vector<double> > bwd;
    initArray(bwd, numStates, T);
/* Basisfall */
    for (int i = 0; i < numStates; i++)
        bwd[i][T - 1] = 1;
/* Induktion */
    for (unsigned long t = T - 2; t >= 0; t--) {
        for (int i = 0; i < numStates; i++) {
            bwd[i][t] = 0;
            for (int j = 0; j < numStates; j++)
                bwd[i][t] += (bwd[j][t + 1] * a[i][j] *
                              b[j][o[t + 1]]);
        }
    }
    return bwd;
}

/** Berechnet die Wahrscheinlichkeit P(X_t = s_i, X_t+1 = s_j | O, m).
@param t der Zeitpunkt t
@param i die Nummer des Zustandes s_i
@param j die Nummer des Zustandes s_j
@param o eine Realisation o
@param fwd die Forward-Variablen f ̈ur o
@param bwd die Backward-Variablen f ̈ur o
@return P
*/
double HMM::p(unsigned long t, unsigned long i, unsigned long j, vector<bool> o, vector<vector<double> > fwd,
              vector<vector<double> > bwd) {
    double num;
    if (t == o.size() - 1)
        num = fwd[i][t] * a[i][j];
    else
        num = fwd[i][t] * a[i][j] * b[j][o[t + 1]] * bwd[j][t + 1];
    double denom = 0;
    for (int k = 0; k < numStates; k++)
        denom += (fwd[k][t] * bwd[k][t]);
    return divide(num, denom);
}

/** Berechnet gamma(i, t) */
double
HMM::gamma(unsigned long i, unsigned long t, vector<bool> o, vector<vector<double> > fwd, vector<vector<double> > bwd) {
    double num = fwd[i][t] * bwd[i][t];
    double denom = 0;
    for (int j = 0; j < numStates; j++)
        denom += fwd[j][t] * bwd[j][t];
    return divide(num, denom);
}

/** Berechnet die Forward-Variablen f(i,t) f ̈ur Zust ̈ande i zum Zeitpunkt
t unter einer Realisation O und der aktuellen Parameterbelegung
@param o die Realisation O
@return ein Verbund f(i,t)  ̈uber die Zust ̈ande und die Zeit, der die
Forward-variablen enth ̈alt.
*/
vector<vector<double> > HMM::forwardProc(vector<bool> o) {
    unsigned long T = o.size();
    vector<vector<double> > fwd;
    initArray(fwd, numStates, T);
/* Basisfall */
    for (int i = 0; i < numStates; i++)
        fwd[i][0] = pi[i] * b[i][o[0]];
/* Induktion */
    for (int t = 0; t <= T - 2; t++) {
        for (int j = 0; j < numStates; j++) {
            fwd[j][t + 1] = 0;
            for (int i = 0; i < numStates; i++)
                fwd[j][t + 1] += (fwd[i][t] * a[i][j]);
            fwd[j][1] *= b[j][o[t + 1]];
        }
    }
    return fwd;
}

/** Dividiert zwei Doubles. 0 / 0 = 0! */
double HMM::divide(double n, double d) {
    if (n == 0)
        return 0;
    else
        return n / d;
}

/** Druckt alle Parameter eines HMM */
void HMM::print() {
    for (int i = 0; i < numStates; i++)
        printf("pi(%d) = %.2f\n", i, pi[i]);
    for (int i = 0; i < numStates; i++) {
        for (int j = 0; j < numStates; j++)
            printf("a(%d,%d) = %.2f  ", i, j, a[i][j]);
        printf("\n");
    }
    printf("\n");
    for (int i = 0; i < numStates; i++) {
        for (int k = 0; k < sigmaSize; k++)
            printf("b(%d,%d) = %.2f  ", i, k, b[i][k]);
        printf("\n");
    }
}


