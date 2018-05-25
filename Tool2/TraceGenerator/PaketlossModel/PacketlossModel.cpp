//
// Created by woors on 22.05.2018.
//

#include <stdexcept>
#include <vector>
#include "PacketlossModel.h"

float PacketlossModel::generateRandomNumber() {
    float max_random_number = 1.0;
    return(static_cast <float> (rand()) / (RAND_MAX / max_random_number));
}
