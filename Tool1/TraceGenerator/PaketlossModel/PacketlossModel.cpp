
#include <stdexcept>
#include <vector>
#include "PacketlossModel.h"
#include <iostream>
float PacketlossModel::generateRandomNumber() {
    return(distribution(generator));
}
