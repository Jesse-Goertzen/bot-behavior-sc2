#include "UnitManager.h"
#include "StateMachineManager.h"


// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L158
// Count the types of the passed unit
size_t UnitManager::CountUnitType(const sc2::ObservationInterface* observation, sc2::UnitTypeID unit_type) {
    return observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(unit_type)).size();
}

bool UnitManager::BuildDrone(const sc2::ObservationInterface* observation, sc2::ActionInterface* actions) {

    if (observation->GetMinerals() < 50) {
        // std::cout << "Not enough minerals to train drone! (" << Observation()->GetMinerals() << "/50)" << std::endl;
        return false;
    }

    if (CountUnitType(observation, sc2::UNIT_TYPEID::ZERG_LARVA) == 0) {
        // std::cout << "Not enough larva to train drone!" << std::endl;
        return false;
    }

    if (getAvailableSupply(observation) < 1) {
        // std::cout << "Not enough supply to train drone!" << std::endl;
        return false;
    }

    // Attempt to build
    actions->UnitCommand(getLarva().front(), sc2::ABILITY_ID::TRAIN_DRONE);

    return true;
}

bool UnitManager::BuildOverlord(const sc2::ObservationInterface* observation, sc2::ActionInterface* actions) {

    if (observation->GetMinerals() < 100) {
        // std::cout << "Not enough minerals to train overlord! (" << observation_->GetMinerals() << "/100)" << std::endl;
        return false;
    }

    if (CountUnitType(observation, sc2::UNIT_TYPEID::ZERG_LARVA) == 0) {
        // std::cout << "Not enough larva to train overlord!" << std::endl;
        return false;
    }

    // Attempt to build
    actions->UnitCommand(getLarva().front(), sc2::ABILITY_ID::TRAIN_OVERLORD);

    return true;
}

// Return a list of all the larva
std::vector<const sc2::Unit*> UnitManager::getLarva() {
        return larva;
}


// Update the vectors of the units we have avalible
void UnitManager::UpdateUnits(const sc2::ObservationInterface* observation) {

    // Update the larva vector
    larva = observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_LARVA;
    });

    // Update the drones vector
    drones = observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_DRONE;
    });

    // Update overlords
    overlords = observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_OVERLORD;
    });
}