#include "BasicSc2Bot.h"

void BasicSc2Bot::OnGameStart() {

    std::cout << "Script starting " << std::endl;
}


// Right now it uses a bunch of and statments which is very bad but its just for testing. Using a queue or a state machine would be a better approach
// state machine involves defining an enum that represents each step, and then using a single variable to track the current state. 


// Our on step function will do these sequences of steps as a test
// following this video https://www.youtube.com/watch?v=Rfwzf_AJgFU
// First, once we reach 50 minerals we will make a drone
bool first_drone_built = false;
// Once that is trained we make a new overlord to have more room for more units
bool first_overlord_built = false;
// Make another drone after that overlord. Check that we have 50 minerals as a just in case
bool second_drone_built = false;
// On 16 supply make a hatchery at the nearest location. This costs the drone and 300 minerals
bool first_hatchery_built = false;


void BasicSc2Bot::OnStep() {

    // First, once we reach 50 minerals we will make a drone
    // Check if enough minerals are available
    if (Observation()->GetMinerals() >= 50 && first_drone_built == false) {
        std::cout << "Build first drone..." << std::endl;
        // Get a list of larva
        auto larvae = Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
                return unit.unit_type == sc2::UNIT_TYPEID::ZERG_LARVA;
            });
        // If there is at least one Larva train it into a drone
        if (!larvae.empty()) {
            Actions()->UnitCommand(larvae.front(), sc2::ABILITY_ID::TRAIN_DRONE);
            first_drone_built = true;
            std::cout << "Built first drone!" << std::endl;
        }
    }

    // Once that is trained we make a new overlord to have more room for more units
    // Check if we have 100 for an overlord
    if (Observation()->GetMinerals() >= 100 && first_overlord_built == false && first_drone_built == true) {
        std::cout << "Build first overlord..." << std::endl;
        // Get a list of larva
        auto larvae = Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
                return unit.unit_type == sc2::UNIT_TYPEID::ZERG_LARVA;
            });
        // If there is at least one Larva train it into a drone
        if (!larvae.empty()) {
            Actions()->UnitCommand(larvae.front(), sc2::ABILITY_ID::TRAIN_OVERLORD);
            first_overlord_built = true;
            std::cout << "Built first overlord!" << std::endl;
        }
    }

    // Make another drone after that overlord. Check that we have 50 minerals as a just in case
    if (Observation()->GetMinerals() >= 50 && second_drone_built == false && first_overlord_built == true) {
        std::cout << "Build second drone..." << std::endl;
        // Get a list of larva
        auto larvae = Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
                return unit.unit_type == sc2::UNIT_TYPEID::ZERG_LARVA;
            });
        // If there is at least one Larva train it into a drone
        if (!larvae.empty()) {
            Actions()->UnitCommand(larvae.front(), sc2::ABILITY_ID::TRAIN_DRONE);
            second_drone_built = true;
            std::cout << "Built second drone!" << std::endl;
        }
    }

    if (Observation()->GetMinerals() >= 300 && first_hatchery_built == false && second_drone_built == true) {
        std::cout << "Build Hatchery..." << std::endl;
        BuildHatchery();
        std::cout << "Built Hatchery!" << std::endl;
    }


}

void BasicSc2Bot::BuildHatchery() {
    if (Observation()->GetMinerals() < 300) {
        return; // Not enough minerals to build a Hatchery
    }

    // Find an drone to use
    auto drones = Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_DRONE;
    });
    if (drones.empty()) {
        return;
    }

    // Find the nearest expansion location
    sc2::Point2D hatchery_location = GetNearestExpansion();

    // Send the drone to build a Hatchery at the nearest location
    Actions()->UnitCommand(drones.front(), sc2::ABILITY_ID::BUILD_HATCHERY, hatchery_location);
    std::cout << "Building Hatchery at nearest expansion location!" << std::endl;
    first_hatchery_built = true;
}