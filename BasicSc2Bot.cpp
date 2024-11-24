#include "BasicSc2Bot.h"

#include "header_files.h"
#include "utility.h"

#include "StateMachineManager.h"
#include "UnitManager.h"
#include "BuildingManager.h"

void BasicSc2Bot::OnGameStart() {

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
    // On each step update the units in the unit manager
    unit_manager.UpdateUnits(*this);

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
            break;
        
        case StateMachineManager::WAIT_FOR_HATCHERY: 
            // Wait for hatchery to finish and do other things
            state_machine.WaitForHatchery(*this);
            break;

            case StateMachineManager::IDLE:
            break;
    }
}