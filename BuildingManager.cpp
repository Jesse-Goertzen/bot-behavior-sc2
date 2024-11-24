#include "BuildingManager.h"

// Attempt to build a structure on creep from a random location on the creep
// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L1363
bool BuildingManager::TryBuildOnCreep(BasicSc2Bot& bot, sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type) {
    float rx = sc2::GetRandomScalar();
    float ry = sc2::GetRandomScalar();
    sc2::Point2D build_location = sc2::Point2D(bot.GetStartLocation().x + rx * 15, bot.GetStartLocation().y + ry * 15);

    if (bot.observation->HasCreep(build_location)) {
        return TryBuildStructure(bot, ability_type_for_structure, unit_type, build_location, false);
    }
    return false;
}

// https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.cc#L390
// Expands to nearest location and updates the start location to be between the new location and old bases.
bool BuildingManager::TryExpand(BasicSc2Bot& bot, sc2::AbilityID build_ability, sc2::UnitTypeID worker_type) {
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
bool BuildingManager::TryBuildStructure(BasicSc2Bot& bot, sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Point2D location, bool isExpansion = false) {
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