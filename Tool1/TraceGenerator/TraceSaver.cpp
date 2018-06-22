
#include "TraceSaver.h"
#include <fstream>
#include <vector>

void TraceSaver::writeTraceToFile(vector<bool> trace, string outputFile) {
    fstream file;
    file.open(outputFile, ios::out);
    for (auto &&i : trace) {
        file << i;
    }
    file.close();
}
