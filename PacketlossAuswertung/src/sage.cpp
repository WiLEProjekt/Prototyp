#include <sage.h>

void readMatrixFile(vector <vector<double>> &transitionmatrix, vector <vector<double>> &emissionmatrix,
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