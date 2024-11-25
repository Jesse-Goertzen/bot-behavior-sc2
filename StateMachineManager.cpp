#include "StateMachineManager.h"
#include "UnitManager.h"
#include "BuildingManager.h"
#include "utility.h"

#include "BasicSc2Bot.h"

// Moves us to the next state in the BotState list
void StateMachineManager::completeState() {
    current_state = static_cast<BotState>(current_state + 1);
};

// steps 1 & 2
void StateMachineManager::StartingState(BasicSc2Bot& bot) {
    const size_t DRONE_TARGET = 13;

    // build one drone
    if (bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE) < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }

    // complete the state once we have enough minerals to build an overlord
    if (bot.unit_manager.BuildOverlord(bot)) {
        completeState();
    }
}

// steps 3
void StateMachineManager::PreFirstExpansionState(BasicSc2Bot& bot) {
    const size_t DRONE_TARGET = 16;
    const size_t MINERAL_TARGET = 250; // play with this number, or maybe calculate it based off of how far away the expansion spot will be

    size_t drone_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE);
    if (drone_count < DRONE_TARGET) {
        if (bot.unit_manager.BuildDrone(bot)) {
            ++drone_count;
        }
    }

    if (drone_count == DRONE_TARGET && bot.observation->GetMinerals() >= MINERAL_TARGET) {
        completeState();
    }
}

void StateMachineManager::FirstExpansionState(BasicSc2Bot& bot) {
    // step 4
    if (bot.observation->GetMinerals() > 300) {

        if (bot.building_manager.TryExpand(bot, sc2::ABILITY_ID::BUILD_HATCHERY, sc2::UNIT_TYPEID::ZERG_DRONE)){

            // State is complete
            // std::cout << ">Completed state: " << current_state << std::endl;
            completeState();
        }
        else {
            // std::cout << ">Failed state: " << current_state << std::endl;
        }
        
    }
}

// step 5-7
// objectives:
//          - maintain 16 drones
//          - build an extractor
//          - build a spawning pool
void StateMachineManager::PostFirstExpansionState(BasicSc2Bot& bot) {
    const size_t DRONE_TARGET = 41; // max the first hatchery, and the new extractor
    const size_t EXTRACTOR_TARGET = 1;
    const size_t SPAWN_POOL_TARGET = 1;
    size_t drone_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE);
    size_t extractor_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_EXTRACTOR);
    size_t spawn_pool_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL);

    // spawn an overlord if we can and if its needed
    bot.unit_manager.BuildOverlord(bot);

    // ensure we are worker capped on the one hatchery, in this state our second hatchery shouldn't have finished building yet
    if (drone_count < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
        // ++drone_count;
    }
    
    // build an extractor if we havent started building one yet
    if (extractor_count < 1 && bot.observation->GetMinerals() > 25) {
        sc2::Units bases = bot.observation->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
        for (const auto& base : bases) {
            if (base->build_progress == 1) {
                if (bot.building_manager.TryBuildGas(bot, sc2::ABILITY_ID::BUILD_EXTRACTOR, sc2::UNIT_TYPEID::ZERG_DRONE, base->pos)) {
                    // ++extractor_count;
                }
                // only want to build one extractor right now, once we find the completed hatchery stop checking
                break;
            }
        }
    }

    if (spawn_pool_count < 1 && bot.observation->GetMinerals() > 200) {
        if (bot.building_manager.TryBuildOnCreep(bot, sc2::ABILITY_ID::BUILD_SPAWNINGPOOL, sc2::UNIT_TYPEID::ZERG_DRONE)) {
            // ++spawn_pool_count;
        }
    }

    if (bot.observation->GetUnits(sc2::))
    
    if (drone_count == DRONE_TARGET && extractor_count == EXTRACTOR_TARGET && spawn_pool_count == SPAWN_POOL_TARGET) {
        completeState();
    }
}

// steps: 8, 9, 10
// exit condition: enough gas for lair (100)
void StateMachineManager::QueeningState(BasicSc2Bot& bot) {
    const size_t QUEEN_TARGET = 2;
    const size_t ZERGLING_TARGET = 4; // maybe 0? lol
    const size_t DRONE_TARGET = 41; // wont hit, but basically make as many as possible

    // build overlords as needed and as possible
    bot.unit_manager.BuildOverlord(bot);
}



// // BUILD_FIRST_DRONE, BUILD_SECOND_DRONE, BUILD_THIRD_DRONE etc.
// void StateMachineManager::BuildDrone(BasicSc2Bot& bot) {

//     if (bot.unit_manager.BuildDrone(bot)) {
//         // State is complete
//         std::cout << ">Completed state: " << current_state << std::endl;
//         completeState();
//     }

// }

// // BUILD_FIRST_OVERLORD
// void StateMachineManager::BuildOverlord(BasicSc2Bot& bot) {

//     if (bot.unit_manager.BuildOverlord(bot)) {
//         // State is complete
//         std::cout << ">Completed state: " << current_state << std::endl;
//         completeState();
//     }
// }

// // FIRST_EXPAND
// void StateMachineManager::FirstExpand(BasicSc2Bot& bot) {

//     if (bot.observation->GetMinerals() > 300) {

//         if (bot.building_manager.TryExpand(bot, sc2::ABILITY_ID::BUILD_HATCHERY, sc2::UNIT_TYPEID::ZERG_DRONE)){

//             // State is complete
//             std::cout << ">Completed state: " << current_state << std::endl;
//             completeState();
//         }
//         else {
//             std::cout << ">Failed state: " << current_state << std::endl;
//         }
        
//     }
// }

// // WAIT_FOR_HATCHERY
// void StateMachineManager::WaitForHatchery(BasicSc2Bot& bot) {
    
//     // Get all the town halls
//     sc2::Units town_halls = bot.observation->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());

//     // Max distance from starting place
//     float max_distance = 0.0f;

//     // Find the Hatchery farthest from the starting location.
//     // Will have to change this if we want to select them based of build order or soemthing else
//     for (const auto& town_hall : town_halls) {
//         float distance = Distance2D(bot.GetStartLocation(), town_hall->pos);
//         if (distance > max_distance) {
//             max_distance = distance;
//             bot.first_expansion = town_hall;
//         }
//     }

//     if (bot.first_expansion) {
//         // Check the Hatchery is built.
//         if (bot.first_expansion->build_progress >= 1.0f) {
//             std::cout << "Constucting hatchery... (100%)" << std::endl;
//             std::cout << "CREATED HATCHERY AT " << bot.first_expansion->pos.x << ", " << bot.first_expansion->pos.y << std::endl;

//             bot.actions->UnitCommand(bot.first_expansion, sc2::ABILITY_ID::TRAIN_DRONE);

//             bot.state_machine.current_state = StateMachineManager::IDLE;
//         } 
//         // Progress bar
//         else if (bot.first_expansion->build_progress == 0.25f) {
//             std::cout << "Constucting hatchery... (25%)" << std::endl;
//         }
//         else if (bot.first_expansion->build_progress == 0.5f) {
//             std::cout << "Constucting hatchery... (50%)" << std::endl;
//         }
//         else if (bot.first_expansion->build_progress == 0.75f) {
//             std::cout << "Constucting hatchery... (75%)" << std::endl;
//         }
//     }

//     // While we wait for hatchery, check for when we reach 200 minerals to make a single spawning pool
//     if (bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL) < 1) {
//         if (bot.building_manager.TryBuildOnCreep(bot, sc2::ABILITY_ID::BUILD_SPAWNINGPOOL, sc2::UNIT_TYPEID::ZERG_DRONE)) {
//         }
//     }    
// }