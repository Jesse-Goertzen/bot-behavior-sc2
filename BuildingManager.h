#ifndef BUILDINGMANAGER_H
#define BUILDINGMANAGER_H

#include "header_files.h"

#include "BasicSc2Bot.h"


class BuildingManager {
public:

    bool TryBuildOnCreep(BasicSc2Bot& bot, sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type);

    bool TryExpand(BasicSc2Bot& bot, sc2::AbilityID build_ability, sc2::UnitTypeID worker_type);

    bool TryBuildStructure(BasicSc2Bot& bot, sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Point2D location, bool isExpansion = false);



private:


};

#endif