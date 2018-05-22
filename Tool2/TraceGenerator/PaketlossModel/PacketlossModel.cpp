//
// Created by woors on 22.05.2018.
//

#include "PacketlossModel.h"

string PacketlossModel::generateTrace() {
    string trace = this->buildTrace();
    if (this->checkTrace(trace)) {
        return trace;
    } else{
        throw;
    }
}

bool PacketlossModel::checkTrace(string trace) {
    for (char c : trace) {
        if(c != '0' && c != '1'){
            return false;
        }
    }
    return true;
}
