#include "StateMachineManager.h"
#include "UnitManager.h"
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
        printf("Starting State Done\n");
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

    if (drone_count >= DRONE_TARGET && bot.Observation()->GetMinerals() >= MINERAL_TARGET) {
        completeState();
        printf("PreExpansion State Done\n");
    }
}

void StateMachineManager::FirstExpansionState(BasicSc2Bot& bot) {
    // step 4
    if (bot.Observation()->GetMinerals() > 300) {

        if (bot.unit_manager.TryExpand(bot, sc2::ABILITY_ID::BUILD_HATCHERY, sc2::UNIT_TYPEID::ZERG_DRONE)){
            std::cout << "Expanded State Done" << std::endl;
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
    if (extractor_count < EXTRACTOR_TARGET && bot.Observation()->GetMinerals() > 25) {
        sc2::Units bases = bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
        for (const auto& base : bases) {
            if (base->build_progress == 1) {
                bot.unit_manager.TryBuildGas(bot, sc2::ABILITY_ID::BUILD_EXTRACTOR, sc2::UNIT_TYPEID::ZERG_DRONE, base->pos);
                // only want to build one extractor right now, once we find the completed hatchery stop checking
                break;
            }
        }
    }

    if (spawn_pool_count < 1 && bot.Observation()->GetMinerals() > 200) {
        bot.unit_manager.TryBuildOnCreep(bot, sc2::ABILITY_ID::BUILD_SPAWNINGPOOL, sc2::UNIT_TYPEID::ZERG_DRONE, bot.GetStartLocation());
    }

    // Call drone handler for new buildings
    bot.unit_manager.HandleDrones(bot);
    
    // Once we reach the drone amount, extractor and spawn pool built complete the state
    if (drone_count == DRONE_TARGET && extractor_count == EXTRACTOR_TARGET && spawn_pool_count == SPAWN_POOL_TARGET) {
        completeState();
        printf("Post First Expansion State Done\n");
    }
}

// steps: 8, 9, 10, 11
// exit condition: 36 supply
void StateMachineManager::QueeningState(BasicSc2Bot& bot) {
    const size_t QUEEN_TARGET = 2;
    const size_t ZERGLING_TARGET = 2; // Changed to 2 since they are made in pairs
    const size_t DRONE_TARGET = 41; // wont hit, but basically make as many as possible
    const size_t LAIR_TARGET = 1;
    size_t drone_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE);
    drone_count += bot.unit_manager.CountUnitEggs(bot, sc2::ABILITY_ID::TRAIN_DRONE);
    size_t queen_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_QUEEN);
    size_t zergling_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_ZERGLING);
    zergling_count += bot.unit_manager.CountUnitEggs(bot, sc2::ABILITY_ID::TRAIN_ZERGLING);
    size_t lair_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_LAIR); 
    size_t larva_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_LARVA);

    // Check if a queen is currently being trained, if so add it to the count
    if (bot.unit_manager.IsQueenInTraining(bot)) {
        queen_count++;
    }

    // build overlords as needed and as possible
    bot.unit_manager.BuildOverlord(bot);

    // Morph hatchery into lair
    if (lair_count < LAIR_TARGET) {
        bot.unit_manager.TryMorphToLair(bot);
    }

    if (queen_count < QUEEN_TARGET) {
        bot.unit_manager.BuildQueen(bot);
    }

    if (zergling_count < ZERGLING_TARGET) {
        bot.unit_manager.BuildZergling(bot);
    }

    // Moved drone function down here so we dont just go for drones first... We want to proritize the other two units first
    // Make stop drone count after we hit the used supply target
    if (zergling_count >= ZERGLING_TARGET && drone_count < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }


    // Attempt to inject larva for each queen
    // Plan is to run inject every 29 seconds, but if we just continiously try to inject, it will fail because its already injecting
    // Therfore we dont need to account for 29 seconds and just always call in this function
    bot.unit_manager.TryInjectLarva(bot);

    bot.unit_manager.HandleDrones(bot);

    // Took out drone target as a condition as we just want to produce as much as possible during this state
    // drone_count == DRONE_TARGET && 
    // Add 2 to the zergling target for final check since training 1 spawns 2
    // printf("QState: Q:%zd L:%zd D:%zd Z:%zd, LV:%zd\n", queen_count, lair_count, drone_count, zergling_count, larva_count);
    if (queen_count == QUEEN_TARGET && lair_count == LAIR_TARGET) {
        std::cout << "Queen state done" << std::endl;
        completeState();
    }

}

// step: 12
// exit condition: drones assigned and two extractors built
void StateMachineManager::MoreExtractingState(BasicSc2Bot& bot) { 
    const size_t EXTRACTOR_TARGET = 3; // 2 + the one before
    const size_t DRONE_TARGET = 41;
    size_t extractor_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_EXTRACTOR);
    size_t drone_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE);
    drone_count += bot.unit_manager.CountUnitEggs(bot, sc2::ABILITY_ID::TRAIN_DRONE);
    size_t larva_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_LARVA);

    // Build extractors
    if (extractor_count < EXTRACTOR_TARGET && bot.Observation()->GetMinerals() > 25) {
        sc2::Units bases = bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
        for (const auto& base : bases) {
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

    if (drone_count < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }

    if (extractor_count == EXTRACTOR_TARGET) {
        completeState();
        printf("More Extractor State Done\n");
    }

}

void StateMachineManager::SaturateExtractorsState(BasicSc2Bot& bot) {
    const size_t DRONE_TARGET = 41;
    size_t drone_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE);
    drone_count += bot.unit_manager.CountUnitEggs(bot, sc2::ABILITY_ID::TRAIN_DRONE);

    bot.unit_manager.SaturateExtractors(bot);

    bot.unit_manager.BuildOverlord(bot);
    bot.unit_manager.TryInjectLarva(bot);
    if (drone_count < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }
    bot.unit_manager.HandleDrones(bot);
    bool extractors_saturated = true;
    if (bot.unit_manager.extractors.size() < 3) {
        extractors_saturated = false;
    }
    for (const auto& extractor : bot.unit_manager.extractors) {
        const sc2::Unit* extractor_unit = bot.Observation()->GetUnit(extractor.first);
        // if (!extractor_unit) continue;
        if (extractor_unit->build_progress < 1) {
            extractors_saturated = false;
        }
        // if any extractors are not saturated, try again
        if (extractor_unit->assigned_harvesters < 3) {
            extractors_saturated = false;
        }
    }

    // all extractors are saturated (or destroyed lol) 
    // set to true for testing, was if extractors_saturated instead of true
    
    if (extractors_saturated) {
        printf("Saturation State Complete\n");
        completeState();
    }
}

void StateMachineManager::RoachWarrenState(BasicSc2Bot& bot) {
    const size_t DRONE_TARGET = 41;
    size_t drone_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE);
    drone_count += bot.unit_manager.CountUnitEggs(bot, sc2::ABILITY_ID::TRAIN_DRONE);

    bot.unit_manager.BuildOverlord(bot);
    bot.unit_manager.TryInjectLarva(bot);
    bot.unit_manager.HandleDrones(bot);

    if (drone_count < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }

    if (bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_ROACHWARREN) < 1) {
        bot.unit_manager.TryBuildOnCreep(bot, sc2::ABILITY_ID::BUILD_ROACHWARREN, sc2::UNIT_TYPEID::ZERG_DRONE, bot.GetStartLocation());    
    }   

    if (bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_ROACHWARREN) > 0) {
        printf("Roach Warren State Done\n");
        completeState();
    }
}

void StateMachineManager::BaseDefenseState(BasicSc2Bot& bot) {
    const size_t DRONE_TARGET = 41;
    const size_t SPORE_CRAWLER_TARGET = 2;
    size_t drone_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE);
    drone_count += bot.unit_manager.CountUnitEggs(bot, sc2::ABILITY_ID::TRAIN_DRONE);
    
    bot.unit_manager.BuildOverlord(bot);
    bot.unit_manager.TryInjectLarva(bot);
    bot.unit_manager.HandleDrones(bot);

    if (bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_SPORECRAWLER) < SPORE_CRAWLER_TARGET) {
        // could be an issue if building the spore crawlers fails for some reason, would have to check where to build
        // if we want one at each hatchery
        for (const auto& base : bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, IsTownHall())) {
            bot.unit_manager.TryBuildOnCreep(bot, sc2::ABILITY_ID::BUILD_SPORECRAWLER, sc2::UNIT_TYPEID::ZERG_DRONE, base->pos);
        }
    }

    if (drone_count < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }

    if (bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_SPORECRAWLER) >= SPORE_CRAWLER_TARGET) {
        printf("Base Defense State Done\n");
        completeState();
    }
}

void StateMachineManager::SecondExpansionState(BasicSc2Bot& bot) {
    const size_t DRONE_TARGET = 41;
    size_t drone_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_DRONE);
    drone_count += bot.unit_manager.CountUnitEggs(bot, sc2::ABILITY_ID::TRAIN_DRONE);
    size_t base_count = bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, IsTownHall()).size();

    bot.unit_manager.BuildOverlord(bot);
    bot.unit_manager.TryInjectLarva(bot);
    
    if (drone_count < DRONE_TARGET) {
        bot.unit_manager.BuildDrone(bot);
    }

    // want 2 hatcheries in addtion to the lair we have
    if (base_count < 3) {
        bot.unit_manager.TryExpand(bot, sc2::ABILITY_ID::BUILD_HATCHERY, sc2::UNIT_TYPEID::ZERG_DRONE);
    }

    if (bot.unit_manager.bases.size() == 3) {
        // when all bases are complete, disable workers from going to the third hatchery
        bot.unit_manager.bases[2].second = false;
        bot.unit_manager.BuildQueen(bot);
    }
    
    // this should happen after we check if the final hatchery is complete because we do not want to assign drones to it
    bot.unit_manager.HandleDrones(bot);

    if (drone_count >= DRONE_TARGET && base_count >= 3) {
        printf("Second Expansion State Complete\n");
        completeState();
    }
}

void StateMachineManager::RoachpocalypseState(BasicSc2Bot &bot) {
    size_t larva_count = bot.unit_manager.CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_LARVA);

    // Get the roach warren
    const sc2::Unit* roach_warren = nullptr;
    for (const auto& unit : bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
            return unit.unit_type == sc2::UNIT_TYPEID::ZERG_ROACHWARREN;
        })) {
        if (unit->build_progress == 1.0 && unit->orders.empty()) {
            roach_warren = unit;
            break;
        }
    }

    if (bot.Observation()->GetUpgrades().empty() && roach_warren) {
        bot.Actions()->UnitCommand(roach_warren, sc2::ABILITY_ID::RESEARCH_GLIALREGENERATION);
    }

    bot.unit_manager.HandleDrones(bot);
    bot.unit_manager.BuildOverlord(bot);
    bot.unit_manager.TryInjectLarva(bot);

    if (larva_count > 0) {
        bot.unit_manager.BuildRoach(bot);
    }
}