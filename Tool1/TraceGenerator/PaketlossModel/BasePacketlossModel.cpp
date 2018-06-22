
#include <stdexcept>
#include <vector>
#include "BasePacketlossModel.h"
#include <iostream>
float BasePacketlossModel::generateRandomNumber() {
    return(distribution(generator));
}
