#include "UnitManager.h"
#include "StateMachineManager.h"
#include "BuildingManager.h"

#include "BasicSc2Bot.h"
#include "utility.h"

// #include "sc2_unit_filters.h"

// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L158
// Count the types of the passed unit
size_t UnitManager::CountUnitType(BasicSc2Bot& bot, sc2::UnitTypeID unit_type) {
    if (unit_type == sc2::UNIT_TYPEID::ZERG_DRONE) {
        size_t drone_count = 0;
        // count the number of eggs evolving to drones too
        sc2::Units eggs = bot.observation->GetUnits(sc2::Unit::Self, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_EGG));
        for (const auto& egg : eggs) {
            if (!egg->orders.empty()) {
                if (egg->orders.front().ability_id == sc2::ABILITY_ID::TRAIN_DRONE) {
                    drone_count++;
                }
            }
        }

        drone_count += bot.observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(unit_type)).size();
        return drone_count;
    }
    return bot.observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(unit_type)).size();
}



bool UnitManager::BuildDrone(BasicSc2Bot& bot) {

    if (bot.observation->GetMinerals() < 50) {
        // std::cout << "Not enough minerals to train drone! (" << Observation()->GetMinerals() << "/50)" << std::endl;
        return false;
    }

    if (CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_LARVA) == 0) {
        // std::cout << "Not enough larva to train drone!" << std::endl;
        return false;
    }

    if (getAvailableSupply(bot) < 1) {
        // std::cout << "Not enough supply to train drone!" << std::endl;
        return false;
    }

    // Attempt to build
    bot.actions->UnitCommand(getLarva().front(), sc2::ABILITY_ID::TRAIN_DRONE);

    return true;
}

bool UnitManager::BuildOverlord(BasicSc2Bot& bot) {

    if (bot.observation->GetMinerals() < 100) {
        // std::cout << "Not enough minerals to train overlord! (" << observation_->GetMinerals() << "/100)" << std::endl;
        return false;
    }

    if (CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_LARVA) == 0) {
        // std::cout << "Not enough larva to train overlord!" << std::endl;
        return false;
    }

    // spawn an overlord when appropriate, if we are well below our food cap, no need to spawn an overlord
    if (bot.observation->GetFoodUsed() >= bot.observation->GetFoodCap() - 4) {
        bot.actions->UnitCommand(getLarva().front(), sc2::ABILITY_ID::TRAIN_OVERLORD);
        return true;
    }

    return false;
}

// Update the vectors of the units we have avalible
void UnitManager::UpdateUnits(BasicSc2Bot& bot) {

    // Update the larva vector
    larva = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_LARVA;
    });

    // Update the drones vector
    drones = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_DRONE;
    });

    // Update overlords
    overlords = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_OVERLORD;
    });

    hatcheries = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_HATCHERY;
    });

    queens = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_QUEEN;
    });

    roaches = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_ROACH;
    });
    
    zerglings = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_ZERGLING;
    });
}


void HandleQueenLarvae(BasicSc2Bot& bot) {

}

void AssignDroneToAvailableExtractor(BasicSc2Bot& bot, )

// void UnitManager::HandleIdleDrones(BasicSc2Bot& bot) {
//    // Assume that update units has been called

// }

// // Mine the nearest mineral to Town hall.
// // If we don't do this, probes may mine from other patches if they stray too far from the base after building.
// void UnitManager::MineIdleWorkers(BasicSc2Bot& bot, const sc2::Unit* worker, sc2::AbilityID worker_gather_command, sc2::UnitTypeID vespene_building_type) {
//     sc2::Units bases = bot.observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsTownHall());
//     sc2::Units geysers = bot.observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(vespene_building_type));

//     const sc2::Unit* valid_mineral_patch = nullptr;

//     if (bases.empty()) {
//         return;
//     }

//     for (const auto& geyser : geysers) {
//         if (geyser->assigned_harvesters < geyser->ideal_harvesters) {
//             bot.actions->UnitCommand(worker, worker_gather_command, geyser);
//             return;
//         }
//     }
//     //Search for a base that is missing workers.
//     for (const auto& base : bases) {
//         //If we have already mined out here skip the base.
//         if (base->ideal_harvesters == 0 || base->build_progress != 1) {
//             continue;
//         }
//         if (base->assigned_harvesters < base->ideal_harvesters) {
//             valid_mineral_patch = FindNearestMineralPatch(bot, base->pos);
//             bot.actions->UnitCommand(worker, worker_gather_command, valid_mineral_patch);
//             return;
//         }
//     }

//     if (!worker->orders.empty()) {
//         return;
//     }

//     //If all workers are spots are filled just go to any base.
//     // const Unit* random_base = GetRandomEntry(bases);
//     // valid_mineral_patch = FindNearestMineralPatch(random_base->pos);
//     // Actions()->UnitCommand(worker, worker_gather_command, valid_mineral_patch);
// }


// const sc2::Unit* UnitManager::FindNearestMineralPatch(BasicSc2Bot& bot, const sc2::Point2D& start) {
//     sc2::Units units = bot.observation->GetUnits(sc2::Unit::Alliance::Neutral);
//     float distance = std::numeric_limits<float>::max();
//     const sc2::Unit* target = nullptr;
//     for (const auto& u : units) {
//         if (u->unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
//             float d = DistanceSquared2D(u->pos, start);
//             if (d < distance) {
//                 distance = d;
//                 target = u;
//             }
//         }
//     }
//     //If we never found one return false;
//     if (distance == std::numeric_limits<float>::max()) {
//         return target;
//     }
//     return target;
// }