#include "UnitManager.h"
#include "StateMachineManager.h"
#include "BuildingManager.h"

#include "BasicSc2Bot.h"
#include "utility.h"

void UnitManager::OnGameStart(BasicSc2Bot& bot) {
    // get all hatcheries, should be just one. Need to store its tag
    sc2::Units hatcheries = bot.Observation()->GetUnits(sc2::Unit::Self, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_HATCHERY));
    // store the starting hatchery's tag, and enable workers for the base
    bases.push_back({hatcheries.front()->tag, true});
}


// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L158
// Count the types of the passed unit
size_t UnitManager::CountUnitType(BasicSc2Bot& bot, sc2::UnitTypeID unit_type) {
    return bot.observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(unit_type)).size();
}

// Count the eggs that are training drones
// size_t UnitManager::CountDroneEggs(BasicSc2Bot& bot) {
//     size_t egg_count = 0;
//     sc2::Units eggs = bot.observation->GetUnits(sc2::Unit::Self, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_EGG));
//     for (const auto& egg : eggs) {
//         if (!egg->orders.empty()) {
//             if (egg->orders.front().ability_id == sc2::ABILITY_ID::TRAIN_DRONE) {
//                 egg_count++;
//             }
//         }
//     }
//     return egg_count;
// }

// Count the eggs that are training that unit
size_t UnitManager::CountUnitEggs(BasicSc2Bot& bot, sc2::ABILITY_ID ability) {
    size_t egg_count = 0;
    sc2::Units eggs = bot.observation->GetUnits(sc2::Unit::Self, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_EGG));
    for (const auto& egg : eggs) {
        if (!egg->orders.empty()) {
            if (egg->orders.front().ability_id == ability) {
                egg_count++;
            }
        }
    }
    return egg_count;
}

// Make drone
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
    const sc2::Unit* larva = bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_LARVA)).front();
    bot.actions->UnitCommand(larva, sc2::ABILITY_ID::TRAIN_DRONE);

    return true;
}

// Make overlord
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
        const sc2::Unit* larva = bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_LARVA)).front();
        bot.actions->UnitCommand(larva, sc2::ABILITY_ID::TRAIN_OVERLORD);
        return true;
    }

    return false;
}

// Make queen
bool UnitManager::BuildQueen(BasicSc2Bot& bot) { 
    // ensure sufficient minerals
    if (bot.Observation()->GetMinerals() < 150) {
        return false; 
    }

    // ensure there is atleast 1 spawning pool
    if (bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) { return unit.unit_type == sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL; }).empty()) {
        return false; 
    }

    const sc2::Unit* hatchery = nullptr;
    for (const auto& base : bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return (unit.unit_type == sc2::UNIT_TYPEID::ZERG_HATCHERY ||
                unit.unit_type == sc2::UNIT_TYPEID::ZERG_LAIR ||
                unit.unit_type == sc2::UNIT_TYPEID::ZERG_HIVE) &&
                unit.orders.empty();
    })) {
        hatchery = base;
        break;
    }

    // ensure hatchery was set / available
    if (!hatchery) {
        return false;
    }

    // attempt to build queen
    bot.Actions()->UnitCommand(hatchery, sc2::ABILITY_ID::TRAIN_QUEEN);
    return true;
}

// Make a zergling. Pre much copy of queen function as both need spawning pool but less minerals. Also need to use a larva tho
bool UnitManager::BuildZergling(BasicSc2Bot& bot) { 
    // ensure sufficient minerals
    if (bot.Observation()->GetMinerals() < 50) {
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

    // ensure there is at least 1 spawning pool
    if (bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) { return unit.unit_type == sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL; }).empty()) {
        return false; 
    }

    const sc2::Unit* hatchery = nullptr;
    for (const auto& base : bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return (unit.unit_type == sc2::UNIT_TYPEID::ZERG_HATCHERY ||
                unit.unit_type == sc2::UNIT_TYPEID::ZERG_LAIR ||
                unit.unit_type == sc2::UNIT_TYPEID::ZERG_HIVE) &&
                unit.orders.empty();
    })) {
        hatchery = base;
        break;
    }

    // ensure hatchery was set / available
    if (!hatchery) {
        return false;
    }

    // If we need, add something here that checks for the larva being selected as the one closest to var hatchery
    // Worst case is a larva has to walk to the other base with spawning pool but thats not that big a deal

    // attempt to build zergling
    const sc2::Unit* larva = bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_LARVA)).front();
    if (!larva) {
        // Larva killed
        return false;
    }
    bot.Actions()->UnitCommand(larva, sc2::ABILITY_ID::TRAIN_ZERGLING);
    return true;
}

// Update the vectors of the units we have avalible
void UnitManager::UpdateUnits(BasicSc2Bot& bot) {

    // Update the larva vector
    // larva = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
    //     return unit.unit_type == sc2::UNIT_TYPEID::ZERG_LARVA;
    // });

    // // Update the drones vector
    // drones = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
    //     return unit.unit_type == sc2::UNIT_TYPEID::ZERG_DRONE;
    // });

    // // Update overlords
    // overlords = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
    //     return unit.unit_type == sc2::UNIT_TYPEID::ZERG_OVERLORD;
    // });

    // hatcheries = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
    //     return unit.unit_type == sc2::UNIT_TYPEID::ZERG_HATCHERY;
    // });

    // queens = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
    //     return unit.unit_type == sc2::UNIT_TYPEID::ZERG_QUEEN;
    // });

    // roaches = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
    //     return unit.unit_type == sc2::UNIT_TYPEID::ZERG_ROACH;
    // });
    
    // zerglings = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
    //     return unit.unit_type == sc2::UNIT_TYPEID::ZERG_ZERGLING;
    // });

    // extractors = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
    //     return unit.unit_type == sc2::UNIT_TYPEID::ZERG_EXTRACTOR;
    // });
    
}

// Assigns any excess drones to bases that are under cap
void UnitManager::HandleDrones(BasicSc2Bot& bot) {
    sc2::Units all_drones = bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_DRONE));
    sc2::Units available_drones = {};

    // check current bases for any excess drones, either above the cap or if they are disbled
    for (const auto& base : bases) {
        const sc2::Unit* base_unit = bot.Observation()->GetUnit(base.first);
        // If base is destroyed, skip
        if (!base_unit) {
            // std::cout << "INVALID BASE UNIT" << std::endl;
            continue;
        }
        // If no drones break
        if (all_drones.empty()) {
            break;
        }
        if (base_unit->build_progress < 1) {
            // dont check in progess bases
            continue;
        }
        // if base is not enabled for workers
        if (!base.second && base_unit->assigned_harvesters > 0) {
            // all drones at a disabled base are free to be reassigned
            for (const auto& drone : all_drones) {
                if (drone->orders.empty() || drone->orders.front().target_unit_tag == base.first) {
                    available_drones.push_back(drone);
                }
            }
            continue;
        }
        // all drones above the ideal for a base are free to be reassigned
        if (base_unit->assigned_harvesters > base_unit->ideal_harvesters) {
            int excess_drones = base_unit->assigned_harvesters - base_unit->ideal_harvesters;
            for (const auto& drone : all_drones) {
                if (drone->orders.empty() || drone->orders.front().target_unit_tag == base_unit->tag) {
                    available_drones.push_back(drone);
                    --excess_drones;
                    if (excess_drones == 0) break;
                }
            }
        }
    }

    // check the extractors for any excess drones, either above the cap or if they are disabled
    for (const auto& extractor : extractors) {
        const sc2::Unit *extractor_unit = bot.Observation()->GetUnit(extractor.first);
        // If extractor is destroyed, skip
        if (!extractor_unit) {
            continue;
        }
        // any disabled for havest extractors can have their drones reassigned
        if (!extractor.second && extractor_unit->assigned_harvesters > 0) {
            for (const auto& drone : all_drones) {
                if (!drone) {
                    continue;
                }
                if (drone->orders.empty() || drone->orders.front().target_unit_tag == extractor_unit->tag) {
                    available_drones.push_back(drone);
                }
            }
            continue;
        }

        // dont need to check building in progress extractors
        if (extractor_unit->build_progress < 1) {
            continue;
        }

        // all drones above the ideal are free to be reassigned
        if (extractor_unit->assigned_harvesters > extractor_unit->ideal_harvesters) {
            int excess_drones = extractor_unit->assigned_harvesters - extractor_unit->ideal_harvesters;
            for (const auto& drone : all_drones) {
                if (!drone) {
                    continue;
                }
                if (drone->orders.empty() || drone->orders.front().target_unit_tag == extractor_unit->tag) {
                    available_drones.push_back(drone);
                    --excess_drones;
                    if (excess_drones == 0) break;
                }
            }
        }
    }
    
    for (const auto& extractor : extractors) {
        const sc2::Unit* extractor_unit = bot.Observation()->GetUnit(extractor.first);
        if (!extractor_unit) {
            continue;
        }
        // extractor disabled
        if (!extractor.second) continue; 
        // extractor is saturated already
        if (extractor_unit->assigned_harvesters == extractor_unit->ideal_harvesters) continue;

        int harvester_deficit = extractor_unit->ideal_harvesters - extractor_unit->assigned_harvesters;
        while (harvester_deficit > 0) {
            if (available_drones.empty()) break;
            const sc2::Unit* drone = available_drones.back();
            if (!drone) {
                // std::cout << "INVALID DRONE UNIT" << std::endl;
                continue;
            }
            available_drones.pop_back();

            bot.Actions()->UnitCommand(drone, sc2::ABILITY_ID::HARVEST_GATHER_DRONE, extractor_unit);
        }
    }

    for (const auto& base : bases) {
        const sc2::Unit* base_unit = bot.Observation()->GetUnit(base.first);
        // If base is destroyed, skip
        if (!base_unit) {
            // std::cout << "INVALID BASE2 UNIT" << std::endl;
            continue;
        }
        // if the base is disabled        
        if (!base.second) continue; 
        // base saturated, all is good
        if (base_unit->assigned_harvesters == base_unit->ideal_harvesters) continue;
        // Find the deficit of units we need
        int harvester_deficit = base_unit->ideal_harvesters - base_unit->assigned_harvesters;
        while (harvester_deficit > 0) {
            // printf("harvester deficit %d\n", harvester_deficit);
            // If we dont have any drones, exit
            if (available_drones.empty()) break;
            const sc2::Unit* drone = available_drones.back();
            if (!drone) {
                continue;
            }
            available_drones.pop_back();
            const sc2::Unit* nearest_minerals = FindNearestMineralPatch(bot, base_unit->pos);
            if (!nearest_minerals) continue;
            bot.Actions()->UnitCommand(drone, sc2::ABILITY_ID::HARVEST_GATHER_DRONE, nearest_minerals);
            --harvester_deficit;
        }
    }

}

// Keep turning larva into roaches, but keep at least 1 larva 
void UnitManager::HandleQueenLarvae(BasicSc2Bot& bot) {



}



void UnitManager::SaturateExtractors(BasicSc2Bot& bot) {
    size_t extractor_count = CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_EXTRACTOR);
    size_t base_count = CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_HATCHERY) + CountUnitType(bot, sc2::UNIT_TYPEID::ZERG_LAIR);
    sc2::Units drones = bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_DRONE));
    size_t drone_count = drones.size();

    if (drone_count >= (base_count * 16 + extractor_count * 3)) {
        // enough for everyone, let the drone handler handle it
        HandleDrones(bot);
        return;
    }

    if (drone_count < extractor_count * 3) {
        // not enough drones for all extractors, not sure what to do?
        return;
    }

    size_t drones_for_bases = drone_count - extractor_count * 3;
    size_t drone_ind = drone_count; 
    if (extractors.size() == 0) return;
    // assign drones to extractors
    for (auto& extractor : extractors) {
        extractor.second = true;
        const sc2::Unit* extractor_unit = bot.Observation()->GetUnit(extractor.first);
        if (extractor_unit->build_progress < 1) continue;
        int harvester_deficit = extractor_unit->ideal_harvesters - extractor_unit->assigned_harvesters;
        if (harvester_deficit <= 0) continue;
        for (int i = 0; i < harvester_deficit; ++i) {
            bot.Actions()->UnitCommand(drones[--drone_ind], sc2::ABILITY_ID::HARVEST_GATHER_DRONE, extractor_unit);
        }
    }
    // loop through remaining drones and assign to each base round robin
    // crashes, def fixable but i dont even know if its needed?

    // for (drone_ind; drone_ind > 0; --drone_ind) {
    //     // printf("second loop\n");
    //     printf("base count: %d; drone_ind: %d; math: %d\n", base_count, drone_ind, drone_ind % base_count);
    //     const sc2::Unit* base_unit = bot.Observation()->GetUnit(bases[drone_ind % (base_count - 1)].first);
    //     bot.Actions()->UnitCommand(drones[drone_ind], sc2::ABILITY_ID::HARVEST_GATHER_DRONE, base_unit);
    // }
} 

// Attempt to build a structure on creep from a random location on the creep
// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L1363
bool UnitManager::TryBuildOnCreep(BasicSc2Bot& bot, 
    sc2::AbilityID ability_type_for_structure, 
    sc2::UnitTypeID unit_type, 
    sc2::Point2D loc) 
{
    float rx = sc2::GetRandomScalar();
    float ry = sc2::GetRandomScalar();
    sc2::Point2D build_location = sc2::Point2D(loc.x + rx * 15, loc.y + ry * 15);

    if (bot.observation->HasCreep(build_location)) {
        return TryBuildStructure(bot, ability_type_for_structure, unit_type, build_location, false);
    }
    return false;
}

// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L390
// Expands to nearest location and updates the start location to be between the new location and old bases.
bool UnitManager::TryExpand(BasicSc2Bot& bot, sc2::AbilityID build_ability, sc2::UnitTypeID worker_type) {
    float minimum_distance = std::numeric_limits<float>::max();
    sc2::Point3D closest_expansion;
    for (const auto& expansion : bot.GetExpansions()) {
        float current_distance = sc2::Distance2D(bot.GetStartLocation(), expansion);
        if (current_distance < .01f) {
            continue;
        }

        if (current_distance < minimum_distance) {
            if (bot.query->Placement(build_ability, expansion)) {
                closest_expansion = expansion;
                minimum_distance = current_distance;
            }
        }
    }
    //only update staging location up till 3 bases.
    if (TryBuildStructure(bot, build_ability, worker_type, closest_expansion, true) && bot.observation->GetUnits(sc2::Unit::Self, IsTownHall()).size() < 4) {
        bot.GetStagingLocation() = sc2::Point3D(((bot.GetStagingLocation().x + closest_expansion.x) / 2), ((bot.GetStagingLocation().y + closest_expansion.y) / 2),
            ((bot.GetStagingLocation().z + closest_expansion.z) / 2));
        return true;
    }
    return false;

}

// Attempt to build structure. Return if it fails or not
// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L316C1-L356C2
bool UnitManager::TryBuildStructure(
        BasicSc2Bot& bot, 
        sc2::AbilityID ability_type_for_structure, 
        sc2::UnitTypeID unit_type, 
        sc2::Point2D location, 
        bool isExpansion = false
    ) 
{
    sc2::Units workers = bot.observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(unit_type));

    // If we have no workers Don't build
    if (workers.empty()) {
        return false;
    }

    // Check to see if there is already a worker heading out to build it
    for (const auto& worker : workers) {
        for (const auto& order : worker->orders) {
            if (order.ability_id == ability_type_for_structure) {
                return false;
            }
        }   
    }

    // If no worker is already building one, get a random worker to build one
    const sc2::Unit* unit = GetRandomEntry(workers);

    // Check to see if unit can make it there
    if (bot.query->PathingDistance(unit, location) < 0.1f) {
        return false;
    }

    if (!isExpansion) {
        for (const auto& expansion : bot.GetExpansions()) {
            if (Distance2D(location, sc2::Point2D(expansion.x, expansion.y)) < 7) {
                return false;
            }
        }
    }

    // Check to see if unit can build there
    if (bot.query->Placement(ability_type_for_structure, location)) {
        bot.actions->UnitCommand(unit, ability_type_for_structure, location);
        return true;
    }
    return false;
}

// https://github.com/Blizzard/s2client-api/blob/master/examples/common/bot_examples.cc#L359
//Try to build a structure based on tag, Used mostly for Vespene, since the pathing check will fail even though the geyser is "Pathable"
bool UnitManager::TryBuildStructure(
        BasicSc2Bot& bot, 
        sc2::AbilityID ability_type_for_structure, 
        sc2::UnitTypeID unit_type, 
        sc2::Tag location_tag
    ) 
{
    sc2::Units workers = bot.observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(unit_type));
    const sc2::Unit* target = bot.observation->GetUnit(location_tag);

    if (workers.empty()) {
        return false;
    }

    // Check to see if there is already a worker heading out to build it
    for (const auto& worker : workers) {
        for (const auto& order : worker->orders) {
            if (order.ability_id == ability_type_for_structure) {
                return false;
            }
        }
    }

    // If no worker is already building one, get a random worker to build one
    const sc2::Unit* unit = GetRandomEntry(workers);

    // Check to see if unit can build there
    if (bot.query->Placement(ability_type_for_structure, target->pos)) {
        bot.actions->UnitCommand(unit, ability_type_for_structure, target);
        return true;
    }
    return false;

}

bool UnitManager::TryBuildGas(
        BasicSc2Bot& bot, 
        sc2::AbilityID build_ability, 
        sc2::UnitTypeID worker_type, 
        sc2::Point2D base_location
    ) 
{
    sc2::Units geysers = bot.observation->GetUnits(sc2::Unit::Alliance::Neutral, IsVespeneGeyser());

    //only search within this radius
    float minimum_distance = 15.0f;
    sc2::Tag closestGeyser = 0;
    for (const auto& geyser : geysers) {
        float current_distance = Distance2D(base_location, geyser->pos);
        if (current_distance < minimum_distance) {
            if (bot.query->Placement(build_ability, geyser->pos)) {
                minimum_distance = current_distance;
                closestGeyser = geyser->tag;
            }
        }
    }

    // In the case where there are no more available geysers nearby
    if (closestGeyser == 0) {
        return false;
    }
    return TryBuildStructure(bot, build_ability, worker_type, closestGeyser);

}

// https://github.com/Blizzard/s2client-api/blob/master/examples/common/bot_examples.cc#L202
const sc2::Unit* UnitManager::FindNearestMineralPatch(BasicSc2Bot&bot, const sc2::Point2D& start) {
    sc2::Units units = bot.Observation()->GetUnits(sc2::Unit::Alliance::Neutral);
    float distance = std::numeric_limits<float>::max();
    const sc2::Unit* target = nullptr;
    for (const auto& u : units) {
        if (u->unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD) {
            float d = DistanceSquared2D(u->pos, start);
            if (d < distance) {
                distance = d;
                target = u;
            }
        }
    }
    //If we never found one return false;
    if (distance == std::numeric_limits<float>::max()) {
        return target;
    }
    return target;
}

// Attempt to inject larva for all queens
void UnitManager::TryInjectLarva(BasicSc2Bot&bot) {

    const sc2::ObservationInterface* observation = bot.Observation();
    sc2::Units queens = observation->GetUnits(sc2::Unit::Alliance::Self, sc2::IsUnit(sc2::UNIT_TYPEID::ZERG_QUEEN));
    sc2::Units hatcheries = observation->GetUnits(sc2::Unit::Alliance::Self,IsTownHall());

     // no queens or no hatcheries
    if (queens.empty() || hatcheries.empty())
        return;

    for (size_t i = 0; i < queens.size(); ++i) {
        for (size_t j = 0; j < hatcheries.size(); ++j) {

            // hatchery isnt complete, so ignore it
            if (hatcheries.at(j)->build_progress != 1) {
                continue;
            }
            else {
                if (i < queens.size()) {
                    if (queens.at(i)->energy >= 25 && queens.at(i)->orders.empty()) {
                        bot.Actions()->UnitCommand(queens.at(i), sc2::ABILITY_ID::EFFECT_INJECTLARVA, hatcheries.at(j));
                        // std::cout << "Injected larva" << std::endl;
                    }
                    ++i;
                }
            }
        }
    }
}

// Change the given hatchery into lair
bool UnitManager::TryMorphToLair(BasicSc2Bot& bot) {
    const sc2::Unit* start_base = nullptr;
    float closest_distance = std::numeric_limits<float>::max();

    // Check mineral and gas
    if (bot.Observation()->GetMinerals() < 150 || bot.Observation()->GetVespene() < 100) {
        return false;
    }

    // Check for a spawning pool anywhere
    if (bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
            return unit.unit_type == sc2::UNIT_TYPEID::ZERG_SPAWNINGPOOL;
        }).empty()) {
        return false; 
    }
    
    // Get the base closest to the start location
    for (const auto& base : bot.Observation()->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
            return unit.unit_type == sc2::UNIT_TYPEID::ZERG_HATCHERY;
        })) {
        float distance = sc2::DistanceSquared2D(base->pos, bot.GetStartLocation());
        if (distance < closest_distance) {
            closest_distance = distance;
            start_base = base;
        }
    }

    if (!start_base) {
        return false;
    }

    // Check if orders are empty
    if (!start_base->orders.empty()) { 
        return false;
    }

    // Morph the Hatchery into a Lair
    bot.Actions()->UnitCommand(start_base, sc2::ABILITY_ID::MORPH_LAIR);
    return true;
}