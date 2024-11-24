#include "StateMachineManager.h"
#include "UnitManager.h"
#include "BuildingManager.h"
#include "utility.h"

#include "BasicSc2Bot.h"

// Moves us to the next state in the BotState list
void StateMachineManager::completeState() {
    current_state = static_cast<BotState>(current_state + 1);
};

// BUILD_FIRST_DRONE, BUILD_SECOND_DRONE, BUILD_THIRD_DRONE etc.
void StateMachineManager::BuildDrone(BasicSc2Bot& bot) {

    if (bot.unit_manager.BuildDrone(bot)) {
        // State is complete
        std::cout << ">Completed state: " << current_state << std::endl;
        completeState();
    }

}

// BUILD_FIRST_OVERLORD
void StateMachineManager::BuildOverlord(BasicSc2Bot& bot) {

    if (bot.unit_manager.BuildOverlord(bot)) {
        // State is complete
        std::cout << ">Completed state: " << current_state << std::endl;
        completeState();
    }
}

// FIRST_EXPAND
void StateMachineManager::FirstExpand(BasicSc2Bot& bot) {

    if (bot.observation->GetMinerals() > 300) {

        if (bot.building_manager.TryExpand(bot, sc2::ABILITY_ID::BUILD_HATCHERY, sc2::UNIT_TYPEID::ZERG_DRONE)){

            // State is complete
            std::cout << ">Completed state: " << current_state << std::endl;
            completeState();
        }
        else {
            std::cout << ">Failed state: " << current_state << std::endl;
        }
        
    }
}

// WAIT_FOR_HATCHERY
void StateMachineManager::WaitForHatchery(BasicSc2Bot& bot) {
    
    // Get all the town halls
    sc2::Units town_halls = bot.observation->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());

    // Max distance from starting place
    float max_distance = 0.0f;

    // Find the Hatchery farthest from the starting location.
    // Will have to change this if we want to select them based of build order or soemthing else
    for (const auto& town_hall : town_halls) {
        float distance = Distance2D(bot.GetStartLocation(), town_hall->pos);
        if (distance > max_distance) {
            max_distance = distance;
            bot.first_expansion = town_hall;
        }
    }

    if (bot.first_expansion) {
        // Check the Hatchery is built.
        if (bot.first_expansion->build_progress >= 1.0f) {
            std::cout << "Constucting hatchery... (100%)" << std::endl;
            std::cout << "CREATED HATCHERY AT " << bot.first_expansion->pos.x << ", " << bot.first_expansion->pos.y << std::endl;

            bot.actions->UnitCommand(bot.first_expansion, sc2::ABILITY_ID::TRAIN_DRONE);

            bot.state_machine.current_state = StateMachineManager::IDLE;
        } 
        // Progress bar
        else if (bot.first_expansion->build_progress == 0.25f) {
            std::cout << "Constucting hatchery... (25%)" << std::endl;
        }
        else if (bot.first_expansion->build_progress == 0.5f) {
            std::cout << "Constucting hatchery... (50%)" << std::endl;
        }
        else if (bot.first_expansion->build_progress == 0.75f) {
            std::cout << "Constucting hatchery... (75%)" << std::endl;
        }
    }

    // While we wait for hatchery, check for when we reach 200 minerals to make a single spawning pool
    if (bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL) < 1) {
        if (bot.building_manager.TryBuildOnCreep(bot, sc2::ABILITY_ID::BUILD_SPAWNINGPOOL, sc2::UNIT_TYPEID::ZERG_DRONE)) {
        }
    }    
}