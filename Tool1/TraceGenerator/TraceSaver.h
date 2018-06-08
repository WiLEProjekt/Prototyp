
#ifndef TRACEGENERATOR_TRACESAVER_H
#define TRACEGENERATOR_TRACESAVER_H

#include <string>
#include <vector>

using namespace std;
class TraceSaver {
public:
    /**
     * Writes a trace in a file
     * @param trace the loss trace
     */
    static void writeTraceToFile(std::vector<bool> trace);
};


#endif //TRACEGENERATOR_TRACESAVER_H
