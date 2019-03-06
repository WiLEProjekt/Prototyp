//
// Created by drieke on 25.01.19.
//

#ifndef TRACEGENERATOR_RESULT_H
#define TRACEGENERATOR_RESULT_H

#include <vector>
#include <algorithm>

using namespace std;

struct result {
    /**
     * 0 = packet arrived, 1 = packet lost
     */
    vector<bool> *loss;
    double reordering;
    double duplication;
    vector<int64_t> *delays;
};

#endif //TRACEGENERATOR_RESULT_H
