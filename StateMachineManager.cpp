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
    drone_count += bot.unit_manager.CountUnitEggs(bot, sc2::ABILITY_ID::TRAIN_DRONE);

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
    drone_count += bot.unit_manager.CountUnitEggs(bot, sc2::ABILITY_ID::TRAIN_DRONE);
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
            std::cout << "Expanded" << std::endl;
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
    drone_count += bot.unit_manager.CountUnitEggs(bot, sc2::ABILITY_ID::TRAIN_DRONE);
    size_t extractor_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_EXTRACTOR);
    size_t spawn_pool_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL);

    // spawn an overlord if we can and if its needed
    bot.unit_manager.BuildOverlord(bot);

    // ensure we are worker capped on the one hatchery, in this state our second hatchery shouldn't have finished building yet
    if (drone_count < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }
    
    // build an extractor if we havent started building one yet
    if (extractor_count < EXTRACTOR_TARGET && bot.observation->GetMinerals() > 25) {
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

// steps: 8, 9, 10, 11
// exit condition: 36 supply
void StateMachineManager::QueeningState(BasicSc2Bot& bot) {
    const size_t QUEEN_TARGET = 2;
    const size_t ZERGLING_TARGET = 2; // Changed to 2 since they are made in pairs
    // const size_t DRONE_TARGET = 42; // wont hit, but basically make as many as possible
    const uint32_t SUPPLY_TARGET = 36;
    const size_t LAIR_TARGET = 1;
    size_t drone_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE);
    drone_count += bot.unit_manager.CountUnitEggs(bot, sc2::ABILITY_ID::TRAIN_DRONE);
    size_t queen_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_QUEEN);
    size_t zergling_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_ZERGLING);
    zergling_count += bot.unit_manager.CountUnitEggs(bot, sc2::ABILITY_ID::TRAIN_ZERGLING);
    size_t lair_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_LAIR); 
    uint32_t supply_count = bot.observation->GetFoodUsed();
    int gas_count = bot.observation->GetVespene();

    // Check if a queen is currently being trained, if so add it to the count
    if (bot.unit_manager.IsQueenInTraining(bot)) {
        queen_count++;
    }

    // build overlords as needed and as possible
    bot.unit_manager.BuildOverlord(bot);

    if (queen_count < QUEEN_TARGET) {
        bot.unit_manager.BuildQueen(bot);
    }

    if (zergling_count < ZERGLING_TARGET) {
        bot.unit_manager.BuildZergling(bot);
    }

    // Moved drone function down here so we dont just go for drones first... We want to proritize the other two units first
    // Make stop drone count after we hit the used supply target
    if (supply_count < SUPPLY_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }

    // Morph hatchery into lair
    if (lair_count < LAIR_TARGET) {
        bot.unit_manager.TryMorphToLair(bot);
    }

    // Attempt to inject larva for each queen
    // Plan is to run inject every 29 seconds, but if we just continiously try to inject, it will fail because its already injecting
    // Therfore we dont need to account for 29 seconds and just always call in this function
    bot.unit_manager.TryInjectLarva(bot);

    bot.unit_manager.HandleDrones(bot);

    // Took out drone target as a condition as we just want to produce as much as possible during this state
    // drone_count == DRONE_TARGET && 
    // Add 2 to the zergling target for final check since training 1 spawns 2
    if (zergling_count == (ZERGLING_TARGET + 2) && queen_count == QUEEN_TARGET && lair_count == LAIR_TARGET && supply_count == SUPPLY_TARGET) {
        std::cout << "Queen done" << std::endl;
        completeState();
    }

}

// step: 12
// exit condition: drones assigned and two extractors built
void StateMachineManager::MoreExtractingState(BasicSc2Bot& bot) { 
    const size_t EXTRACTOR_TARGET = 3; // 2 + the one before
    size_t extractor_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_EXTRACTOR);

    // Build extractors
    if (extractor_count < EXTRACTOR_TARGET && bot.observation->GetMinerals() > 25) {
        sc2::Units bases = bot.observation->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
        for (const auto& base : bases) {
            std::cout << base->pos.x << " " << base->pos.y << std::endl;
            // If base is destroyed dont build
            if (!base) {
                break;
            }

            if (base->build_progress == 1) {
                // If it returns false, out of range for that base and we must loop to next one
                if (bot.unit_manager.TryBuildGas(bot, sc2::ABILITY_ID::BUILD_EXTRACTOR, sc2::UNIT_TYPEID::ZERG_DRONE, base->pos)) {
                    break;
                };
                
            }
        }
    }

    if (extractor_count == EXTRACTOR_TARGET) {
        std::cout << "Done" << std::endl;
        completeState();
    }

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