#include "UnitManager.h"

bool UnitManager::BuildDrone() {

    if (Observation()->GetMinerals() < 50) {
        // std::cout << "Not enough minerals to train drone! (" << Observation()->GetMinerals() << "/50)" << std::endl;
        return false;
    }

    if (larva.empty()) {
        // std::cout << "Not enough larva to train drone!" << std::endl;
        return false;
    }

    if (getAvailableSupply() < 1) {
        // std::cout << "Not enough supply to train drone!" << std::endl;
        return false;
    }

    // Attempt to build
    Actions()->UnitCommand(getLarva().front(), sc2::ABILITY_ID::TRAIN_DRONE);

    return true;
}