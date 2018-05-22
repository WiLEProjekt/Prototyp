//
// Created by woors on 22.05.2018.
//

#ifndef TRACEGENERATOR_MARKOVMODEL_H
#define TRACEGENERATOR_MARKOVMODEL_H


#include "PacketlossModel.h"

using namespace std;
class MarkovModel : PacketlossModel {
public:
    MarkovModel();
    string buildTrace() override;
};


#endif //TRACEGENERATOR_MARKOVMODEL_H
