#include "BasicSc2Bot.h"


void BasicSc2Bot::OnGameStart() {

    std::cout << "Script starting " << std::endl;

    // Set the current state
    current_state = BUILD_FIRST_DRONE;
}

void BasicSc2Bot::OnStep() {
    // On each step update the amount of units. This updates larva, overlord and drone vectors in the class
    // Can get the size of each using larva.size() for example
    UpdateUnits();

    // Current supply
    int current_supply = Observation()->GetFoodUsed();

    // Switch case for all the steps we will do
    switch (current_state) {

        case BUILD_FIRST_DRONE:
            // Attempt to build drone
            if (BuildDrone()) {
                std::cout << "BUILT FIRST DRONE" << std::endl;
                current_state = BUILD_OVERLORD;
            }
            break;

        // Once that is trained we make a new overlord to have more room for more units
        case BUILD_OVERLORD:
            // Attempt to build overlord

            if (BuildOverlord()) {
                std::cout << "BUILT FIRST OVERLORD" << std::endl;
                current_state = IDLE;
            }


        case IDLE:
            // std::cout << "Idle..." << std::endl;

        break;

    }
}

// // Function to build a hatchery from a drone
// bool BasicSc2Bot::BuildHatchery() {

//     // Find an drone to use
//     auto drones = Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
//         return unit.unit_type == sc2::UNIT_TYPEID::ZERG_DRONE;
//     });
//     if (drones.empty()) {
//         return false;
//     }

//     // Find the nearest expansion location
//     sc2::Point2D hatchery_location = GetNearestExpansion();

//     // Send the drone to build a Hatchery at the nearest location
//     Actions()->UnitCommand(drones.front(), sc2::ABILITY_ID::BUILD_HATCHERY, hatchery_location);
//     std::cout << "Built Hatchery!" << std::endl;
//     return true;
// }

// Takes the current state and finds what the next one will be
// BotState BasicSc2Bot::NextState(BotState current) {
//     // using UnderlyingType = std::underlying_type_t<BotState>;
//     // UnderlyingType nextValue = (static_cast<UnderlyingType>(current) + 1) % static_cast<UnderlyingType>(BotState::Count);
//     // return static_cast<BotState>(nextValue);
// }

// Function that attempts to build a drone. Returns true or false if it succeeds
// Should update this to be a general use case for any unit
bool BasicSc2Bot::BuildDrone() {

    if (Observation()->GetMinerals() < 50) {
        std::cout << "Not enough minerals to train drone! (" << Observation()->GetMinerals() << "/50)" << std::endl;
        return false;
    }

    if (larva.empty()) {
        std::cout << "Not enough larva to train drone!" << std::endl;
        return false;
    }

    if (getAvailableSupply() < 1) {
        std::cout << "Not enough supply to train drone!" << std::endl;
        return false;
    }

    // Attempt to build
    Actions()->UnitCommand(larva.front(), sc2::ABILITY_ID::TRAIN_DRONE);

    return true;
}

bool BasicSc2Bot::BuildOverlord() {

    if (Observation()->GetMinerals() < 100) {
        std::cout << "Not enough minerals to train overlord! (" << Observation()->GetMinerals() << "/100)" << std::endl;
        return false;
    }

    if (larva.empty()) {
        std::cout << "Not enough larva to train overlord!" << std::endl;
        return false;
    }

    // Attempt to build
    Actions()->UnitCommand(larva.front(), sc2::ABILITY_ID::TRAIN_OVERLORD);

    return true;
}

// Update the vectors of the units we have avalible
void BasicSc2Bot::UpdateUnits() {

    // Update the larva vector
    larva = Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_LARVA;
    });

    // Update the drones vector
    drones = Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_DRONE;
    });

    // Update overlords
    overlords = Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_OVERLORD;
    });

}

std::vector<const sc2::Unit*> BasicSc2Bot::getLarva() {
        return larva;
}

std::vector<const sc2::Unit*> BasicSc2Bot::getDrones() {
        return drones;
}

// Get the available supply
float BasicSc2Bot::getAvailableSupply() {

    float total_supply = Observation()->GetFoodCap();
    float used_supply = Observation()->GetFoodUsed();
    return total_supply - used_supply;

}