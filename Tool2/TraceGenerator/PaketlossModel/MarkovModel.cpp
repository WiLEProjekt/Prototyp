//
// Created by woors on 22.05.2018.
//

#include <iostream>
#include "MarkovModel.h"

vector<bool> MarkovModel::buildTrace() {
    auto *trace = new vector<bool>();
    for(int i = 0; i < 100; i ++){
        trace->push_back(true);
    }
    return *trace;
}

