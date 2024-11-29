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
    size_t drone_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE);
    drone_count += bot.unit_manager.CountDroneEggs(bot);

    if (drone_count < DRONE_TARGET) {
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
    drone_count += bot.unit_manager.CountDroneEggs(bot);
    if (drone_count < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }

    if (drone_count >= DRONE_TARGET && bot.observation->GetMinerals() >= MINERAL_TARGET) {
        completeState();
    }
}

void StateMachineManager::FirstExpansionState(BasicSc2Bot& bot) {
    // step 4
    if (bot.observation->GetMinerals() > 300) {

        if (bot.unit_manager.TryExpand(bot, sc2::ABILITY_ID::BUILD_HATCHERY, sc2::UNIT_TYPEID::ZERG_DRONE)){
            completeState();
        }
    }
}

// step 5-7
// objectives:
//          - maintain 16 drones
//          - build an extractor
//          - build a spawning pool
void StateMachineManager::PostFirstExpansionState(BasicSc2Bot& bot) {
    const size_t DRONE_TARGET = 19; // max the first hatchery, and the new extractor
    const size_t EXTRACTOR_TARGET = 1;
    const size_t SPAWN_POOL_TARGET = 1;
    size_t drone_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE);
    drone_count += bot.unit_manager.CountDroneEggs(bot);
    size_t extractor_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_EXTRACTOR);
    size_t spawn_pool_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL);

    // spawn an overlord if we can and if its needed
    bot.unit_manager.BuildOverlord(bot);

    // ensure we are worker capped on the one hatchery, in this state our second hatchery shouldn't have finished building yet
    if (drone_count < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }
    
    // build an extractor if we havent started building one yet
    if (extractor_count < 1 && bot.observation->GetMinerals() > 25) {
        sc2::Units bases = bot.observation->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
        for (const auto& base : bases) {
            if (base->build_progress == 1) {
                bot.unit_manager.TryBuildGas(bot, sc2::ABILITY_ID::BUILD_EXTRACTOR, sc2::UNIT_TYPEID::ZERG_DRONE, base->pos);
                // only want to build one extractor right now, once we find the completed hatchery stop checking
                break;
            }
        }
    }

    if (spawn_pool_count < 1 && bot.observation->GetMinerals() > 200) {
        bot.unit_manager.TryBuildOnCreep(bot, sc2::ABILITY_ID::BUILD_SPAWNINGPOOL, sc2::UNIT_TYPEID::ZERG_DRONE, bot.GetStartLocation());
    }

    // Call drone handler for new buildings
    bot.unit_manager.HandleDrones(bot);
    
    // Once we reach the drone amount, extractor and spawn pool built complete the state
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
    size_t drone_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE);
    drone_count += bot.unit_manager.CountDroneEggs(bot);
    size_t queen_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_QUEEN);

    // build overlords as needed and as possible
    bot.unit_manager.BuildOverlord(bot);
    
    if (drone_count < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }

    if (queen_count < QUEEN_TARGET) {
        bot.unit_manager.BuildQueen(bot);
    }

    // Attempt to inject larva for each queen
    // Plan is to run inject every 29 seconds, but if we just continiously try to inject, it will fail because its already injecting
    // Therfore we dont need to account for 29 seconds and just always call in this function
    bot.unit_manager.TryInjectLarva(bot);


    bot.unit_manager.HandleDrones(bot);



}


void StateMachineManager::RoachWarrenState(BasicSc2Bot& bot) {
    const size_t DRONE_TARGET = 41;

    bot.unit_manager.BuildOverlord(bot);
    bot.unit_manager.HandleQueenLarvae(bot);

    if (bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE) < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }

    if (bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_ROACHWARREN) < 1) {
        bot.building_manager.TryBuildOnCreep(bot, sc2::ABILITY_ID::BUILD_ROACHWARREN, sc2::UNIT_TYPEID::ZERG_DRONE, bot.GetStartLocation());    
    }

    if (bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_ROACHWARREN) > 0) {
        completeState();
    }
}

void StateMachineManager::BaseDefenseState(BasicSc2Bot& bot) {
    const size_t DRONE_TARGET = 41;
    const size_t SPORE_CRAWLER_TARGET = 2;

    bot.unit_manager.BuildOverlord(bot);
    bot.unit_manager.HandleQueenLarvae(bot);

    if (bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_SPORECRAWLER) < SPORE_CRAWLER_TARGET) {
        // could be an issue if building the spore crawlers fails for some reason, would have to check where to build
        // if we want one at each hatchery
        for (const auto& hatchery : bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_HATCHERY))) {
            bot.building_manager.TryBuildOnCreep(bot, sc2::ABILITY_ID::BUILD_SPORECRAWLER, sc2::UNIT_TYPEID::ZERG_DRONE, hatchery->pos);
        }
    }

    if (bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE) < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }

    if (bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_SPORECRAWLER) >= SPORE_CRAWLER_TARGET) {
        completeState();
    }
}


// void StateMachineManager::
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