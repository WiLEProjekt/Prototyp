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

void writeBursts(string output, vector<int> &bursts){
    fstream fout;
    fout.open(output, ios::out);
    for(int i = 0; i<bursts.size(); i++){
        fout << bursts[i];
        fout << "\n";
    }
    fout.close();
}