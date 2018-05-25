//
// Created by woors on 22.05.2018.
//

#ifndef TRACEGENERATOR_MARKOVMODEL_H
#define TRACEGENERATOR_MARKOVMODEL_H


#include <vector>
#include "PacketlossModel.h"

using namespace std;
class MarkovModel : public PacketlossModel {
protected:
    vector<bool> buildTrace() override;

public:
    MarkovModel(unsigned int seed) : PacketlossModel(seed) {}
};


#endif //TRACEGENERATOR_MARKOVMODEL_H
