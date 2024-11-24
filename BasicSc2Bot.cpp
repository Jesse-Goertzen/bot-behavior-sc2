#include "BasicSc2Bot.h"

#include "header_files.h"
#include "utility.h"

#include "StateMachineManager.h"
#include "UnitManager.h"
#include "BuildingManager.h"

void BasicSc2Bot::OnGameStart() {

    std::cout << "Script starting " << std::endl;

    // Get the observation object and store it in a pointer
    observation = Observation();

    // Get the action interface and store it as a pointer
    actions = Actions();

    // Query interface store as pointer
    query = Query();

    // Set the current state
    state_machine.current_state = StateMachineManager::BUILD_FIRST_DRONE;

    // Get all expansion locations
    expansions_ = sc2::search::CalculateExpansionLocations(observation, query);

    // https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L153C1-L155C40
    // Set the start location
    startLocation_ = observation->GetStartLocation();
    staging_location_ = startLocation_;

}

void BasicSc2Bot::OnStep() {
    // On each step update the amount of units. This updates larva, overlord and drone vectors in the class
    // Can get the size of each using larva.size() for example
    unit_manager.UpdateUnits(*this);

    // Current supply
    int current_supply = observation->GetFoodUsed();

    sc2::Units town_halls;
    first_expansion_ = nullptr;

    // Max distance is for case of WAIT_FOR_HATCHERY but will remvoe it and put in own function later
    float max_distance = 0.0f;

    // Switch case for all the steps we will do
    switch (state_machine.current_state) {

        // Start by building a drone
        case StateMachineManager::BUILD_FIRST_DRONE:
            // Build first Drone
            state_machine.BuildDrone(*this);
            break;

        case StateMachineManager::BUILD_FIRST_OVERLORD:
            // Build first overlord
            state_machine.BuildOverlord(*this);
            break;

        case StateMachineManager::BUILD_SECOND_DRONE:
            // Build second drone
            state_machine.BuildDrone(*this);
            break;

        
        case StateMachineManager::BUILD_THIRD_DRONE:
            // Build third drone
            state_machine.BuildDrone(*this);
            break;

        case StateMachineManager::FIRST_EXPAND:
            // Attempt to expand
            state_machine.FirstExpand(*this);
            std::cout << "Done" << std::endl;

            break;

        
        case StateMachineManager::WAIT_FOR_HATCHERY: 

        //     // Get all the town halls
        //     town_halls = observation->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());

        //     // Find the Hatchery farthest from the starting location.
        //     // Will have to change this if we want to select them based of build order or soemthing else
        //     for (const auto& town_hall : town_halls) {
        //         float distance = Distance2D(startLocation_, town_hall->pos);
        //         if (distance > max_distance) {
        //             max_distance = distance;
        //             first_expansion_ = town_hall;
        //         }
        //     }

        //     if (first_expansion_) {
        //         // Check the Hatchery is built.
        //         if (first_expansion_->build_progress >= 1.0f) {
        //             std::cout << "Constucting hatchery... (100%)" << std::endl;
        //             std::cout << "CREATED HATCHERY AT " << first_expansion_->pos.x << ", " << first_expansion_->pos.y << std::endl;

        //             actions->UnitCommand(first_expansion_, sc2::ABILITY_ID::TRAIN_DRONE);

        //             state_machine.current_state = StateMachineManager::IDLE;
        //         } 
        //         // Progress bar
        //         else if (first_expansion_->build_progress == 0.25f) {
        //             std::cout << "Constucting hatchery... (25%)" << std::endl;
        //         }
        //         else if (first_expansion_->build_progress == 0.5f) {
        //             std::cout << "Constucting hatchery... (50%)" << std::endl;
        //         }
        //         else if (first_expansion_->build_progress == 0.75f) {
        //             std::cout << "Constucting hatchery... (75%)" << std::endl;
        //         }
        //     }

        //     // While we wait for hatchery, check for when we reach 200 minerals to make a single spawning pool
        //     if (unit_manager.CountUnitType(observation, sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL) < 1) {
        //         if (TryBuildOnCreep(sc2::ABILITY_ID::BUILD_SPAWNINGPOOL, sc2::UNIT_TYPEID::ZERG_DRONE)) {
        //         }
        //     }

            break;


            case StateMachineManager::IDLE:
            break;

    }
}