#include "BasicSc2Bot.h"


// State for each action
enum BotState {
    BUILD_FIRST_DRONE,
    BUILD_OVERLORD,
    BUILD_THREE_DRONE,
    BUILD_FIRST_HATCHERY,
    IDLE
};
BotState current_state = BUILD_FIRST_DRONE;

void BasicSc2Bot::OnGameStart() {

    std::cout << "Script starting " << std::endl;

}

// Right now it uses a bunch of and statments which is very bad but its just for testing. Using a queue or a state machine would be a better approach
// state machine involves defining an enum that represents each step, and then using a single variable to track the current state. 

// Our on step function will do these sequences of steps as a test
// following this video https://www.youtube.com/watch?v=Rfwzf_AJgFU

// First, once we reach 50 minerals we will make a drone

// Once that is trained we make a new overlord to have more room for more units

// Train 3 more drone after that overlord. We will be at 16 supply at this point

// On 16 supply make a hatchery at the nearest location. This costs the drone and 300 minerals
bool first_hatchery_built = false;

void BasicSc2Bot::OnStep() {

    // On each step get the amount of larva
    auto larva = Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_LARVA;
    });

    // Current supply
    int current_supply = Observation()->GetFoodUsed();

    // Switch case for all the steps we will do

    switch (current_state) {

        // First, once we reach 50 minerals we will make a drone
        case BUILD_FIRST_DRONE:
            if (Observation()->GetMinerals() >= 50 && !larva.empty()) {
                std::cout << "STEP>Build Drone<" << std::endl;
                Actions()->UnitCommand(larva.front(), sc2::ABILITY_ID::TRAIN_DRONE);
                current_state = BUILD_OVERLORD;
            }
            break;

        // Once that is trained we make a new overlord to have more room for more units
        case BUILD_OVERLORD:
            if (Observation()->GetMinerals() >= 100 && !larva.empty()) {
                std::cout << "STEP>Build Overlord<" << std::endl;
                Actions()->UnitCommand(larva.front(), sc2::ABILITY_ID::TRAIN_OVERLORD);
                current_state = BUILD_THREE_DRONE;
            }
            break;

        // Train 3 more drone after that overlord. We will be at 16 supply at this point
        // Right now this keeps trying to make more while the others are being trained...
        case BUILD_THREE_DRONE:
            if (Observation()->GetMinerals() >= 50 && !larva.empty()) {
                std::cout << "STEP>Build Drone<" << std::endl;
                Actions()->UnitCommand(larva.front(), sc2::ABILITY_ID::TRAIN_DRONE);
            }
            if (current_supply == 16) {
                std::cout << "Supply at 16..." << std::endl;
                current_state = BUILD_FIRST_HATCHERY;
            }
            break;

        // On 16 supply make a hatchery at the nearest location. This costs the drone and 300 minerals
        case BUILD_FIRST_HATCHERY:
            std::cout << "STEP>Build Hatchery<" << std::endl;
            current_state = IDLE;
            break;

        case IDLE:
            std::cout << "Idle..." << std::endl;

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