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
    bot.actions->UnitCommand(larva.front(), sc2::ABILITY_ID::TRAIN_DRONE);

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
        bot.actions->UnitCommand(larva.front(), sc2::ABILITY_ID::TRAIN_OVERLORD);
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

    extractors = bot.observation->GetUnits(sc2::Unit::Alliance::Self, [](const sc2::Unit& unit) {
        return unit.unit_type == sc2::UNIT_TYPEID::ZERG_EXTRACTOR;
    });
    
}


void HandleQueenLarvae(BasicSc2Bot& bot) {
    // todo
}
