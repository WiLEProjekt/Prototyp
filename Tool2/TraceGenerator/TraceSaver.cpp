
#include "TraceSaver.h"
#include <fstream>
#include <vector>

void TraceSaver::writeTraceToFile(vector<bool> trace) {
    fstream file;
    file.open("trace.txt", ios::out);
    for (auto &&i : trace) {
        file << i;
    }
    file.close();
}
