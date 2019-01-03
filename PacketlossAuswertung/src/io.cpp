#include <io.h>

vector<bool> readBinaryTrace(string filename){
    vector<bool> trace;
    ifstream file;
    file.open(filename, ios::in);
    char c;
    while (file.get(c)) {
        trace.push_back(c != '0');
    }
    trace.pop_back();
    return trace;
}