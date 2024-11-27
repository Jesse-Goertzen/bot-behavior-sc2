#ifndef BUILDING_MANAGER_H
#define BUILDING_MANAGER_H

#include "header_files.h"

class BasicSc2Bot;

class BuildingManager {
public:

    bool TryBuildOnCreep(BasicSc2Bot& bot, sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Point2D loc);

    bool TryExpand(BasicSc2Bot& bot, sc2::AbilityID build_ability, sc2::UnitTypeID worker_type);

    bool TryBuildStructure(BasicSc2Bot& bot, sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Point2D location, bool isExpansion);
    bool TryBuildStructure(BasicSc2Bot& bot, sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Tag location_tag);
    bool TryBuildGas(BasicSc2Bot& bot, sc2::AbilityID build_ability, sc2::UnitTypeID worker_type, sc2::Point2D base_location);
    
private:

    // Max distance is for case of WAIT_FOR_HATCHERY but will remvoe it and put in own function later
    float max_distance = 0.0f;



};

#endif