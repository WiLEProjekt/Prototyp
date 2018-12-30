/*
 * Testscript - ToDo: LÖSCHEN, WENN ES IN TRACEGENERATOR ÜBERNOMMEN WURDE
 * Arguments: Pfad zum Binärtrace, Modellname
 * Startet das Python script
 * Liest dann sageout.json ein
 * Löscht sageout.json dann wieder
 * Gibt dann die Modellparameter aus
*/
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <stdio.h>

using namespace std;

//Reads the temprary file, builds the matrices and deletes the temporary file
void readFile(vector <vector<double>> &transitionmatrix, vector <vector<double>> &emissionmatrix,
              vector<double> &initialprobabilities) {
    ifstream infile;
    string filename = "sageout.txt";
    infile.open(filename);
    string line;
    int i = 0;
    vector<double> temptrans;
    vector<double> tempemission;
    string tempstring;
    //Push all probabilities into tempral vectors, then build the matrices
    while (getline(infile, line)) {
        if (i == 0) { //First line = Transition Matrix
            for (int i = 0; i < line.size(); i++) {
                if (line[i] != ';') {
                    tempstring = tempstring + line[i];
                } else {
                    temptrans.push_back(stod(tempstring));
                    tempstring = "";
                }
            }
        } else if (i == 1) { //Second line = Emission Matrix
            for (int i = 0; i < line.size(); i++) {
                if (line[i] != ';') {
                    tempstring = tempstring + line[i];
                } else {
                    tempemission.push_back(stod(tempstring));
                    tempstring = "";
                }
            }

        } else if (i == 2) { //Third line = Initial Probabilities
            for (int i = 0; i < line.size(); i++) {
                if (line[i] != ';') {
                    tempstring = tempstring + line[i];
                } else {
                    initialprobabilities.push_back(stod(tempstring));
                    tempstring = "";
                }
            }
        } else {
            break;
        }
        i++;
    }
    infile.close();
    //Build Transitionmatrix
    int dimension = sqrt(temptrans.size());
    vector<double> tmp;
    int counter = 0;
    for (int i = 0; i < temptrans.size(); i++) {
        if (counter == dimension) {
            counter = 0;
            transitionmatrix.push_back(tmp);
            tmp.clear();
        }
        tmp.push_back(temptrans[i]);
        counter++;
    }
    transitionmatrix.push_back(tmp);
    tmp.clear();
    counter = 0;

    //Build Emissionmatrix
    for (int i = 0; i < tempemission.size(); i++) {
        if (counter == 2) {
            counter = 0;
            emissionmatrix.push_back(tmp);
            tmp.clear();
        }
        tmp.push_back(tempemission[i]);
        counter++;
    }
    emissionmatrix.push_back(tmp);
    tmp.clear();
    counter = 0;
    if(remove(filename.c_str())!=0){ //delete temporary file
        perror("Error deleting the temporary file");
    }
}

int main(int argc, char **argv) {
    if (argc < 4) {
        cout << "usage: ./starter [path to sagemath python script] [path to binary file] [modelname]" << endl;
        return 0;
    } else {
        string sagescript = argv[1];
        string path = argv[2];
        string model = argv[3];
        if (!(model.compare("gilbert") == 0 || model.compare("gilbertelliot") == 0 || model.compare("markov") == 0)) {
            cout << "wrong model. choose gilbert | gilbertelliot | markov" << endl;
            return 0;
        }
        cout << "Starting Sagemath" << endl;
        string pythonCommand = "sage --python " + sagescript + " " + path + " " + model;
        int sageStatus = system(pythonCommand.c_str());
        if (sageStatus < 0) {
            cout << "An Error occured while executing Sagemath" << endl;
            return 0;
        }
        cout << "Sagemath executed successfully. Calculating Results" << endl;
        vector <vector<double>> transitionmatrix;
        vector <vector<double>> emissionmatrix;
        vector<double> initialprobabilities;
        readFile(transitionmatrix, emissionmatrix, initialprobabilities);

        if (model == "gilbert") {
            cout << "p=" << transitionmatrix[0][1] << " r=" << transitionmatrix[1][0] << " k=" << emissionmatrix[0][1]
                 << " h=" << emissionmatrix[1][1] << endl;
        } else if (model == "gilbertelliot") {
            cout << "p=" << transitionmatrix[0][1] << " r=" << transitionmatrix[1][0] << " k=" << emissionmatrix[0][1]
                 << " h=" << emissionmatrix[1][1] << endl;
        } else if (model == "markov") {
            cout << "p13=" << transitionmatrix[0][2] << " p31=" << transitionmatrix[2][0] << " p32="
                 << transitionmatrix[2][1] << " p23=" << transitionmatrix[1][2] << " p14=" << transitionmatrix[0][3]
                 << " p41=" << transitionmatrix[3][0] << endl;
        }
        /*// Test Output - Matritzen, so wie der Output von Sage auch ist
        cout << "Transition: " << endl;
        for (int i = 0; i < transitionmatrix.size(); i++) {
            for (int b = 0; b < transitionmatrix[i].size(); b++) {
                cout << transitionmatrix[i][b] << " ";
            }
            cout << endl;
        }
        cout << "Emission: " << endl;
        for (int i = 0; i < emissionmatrix.size(); i++) {
            for (int b = 0; b < emissionmatrix[i].size(); b++) {
                cout << emissionmatrix[i][b] << " ";
            }
            cout << endl;
        }
        cout << "Initial Probs: " << endl;
        for (int i = 0; i < initialprobabilities.size(); i++) {
            cout << initialprobabilities[i] << " ";
        }
        cout << endl;*/



    }
}
