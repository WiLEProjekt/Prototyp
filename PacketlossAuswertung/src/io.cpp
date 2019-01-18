#include <io.h>

vector<bool> readBinaryTrace(string filename){
    vector<bool> trace;
    ifstream file;
    file.open(filename, ios::in);
    char c;
    while (file.get(c)) {
        trace.push_back(c != '0');
    }
    return trace;
}

void writeVector(string output, vector<int> &outvec){
    fstream fout;
    fout.open(output, ios::out);
    for(int i = 0; i<outvec.size(); i++){
        fout << outvec[i] << endl;
    }
    fout.close();
}

void writeVector(string output, vector<double> &outvec){
    fstream fout;
    fout.open(output, ios::out);
    for(int i = 0; i<outvec.size(); i++){
        fout << outvec[i] << endl;
    }
    fout.close();
}