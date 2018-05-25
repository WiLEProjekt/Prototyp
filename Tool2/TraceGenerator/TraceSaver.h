
#ifndef TRACEGENERATOR_TRACESAVER_H
#define TRACEGENERATOR_TRACESAVER_H

#include <string>
#include <vector>

using namespace std;
class TraceSaver {
public:
    static void writeTraceToFile(std::vector<bool> trace);
};


#endif //TRACEGENERATOR_TRACESAVER_H
