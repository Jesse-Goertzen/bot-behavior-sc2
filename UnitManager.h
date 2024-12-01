#ifndef UNIT_MANAGER_H
#define UNIT_MANAGER_H

#include "header_files.h"

class BasicSc2Bot;

class UnitManager {
public:
    void OnGameStart(BasicSc2Bot& bot);

    // Return count of the unit type passed
    size_t CountUnitType(BasicSc2Bot& bot, sc2::UnitTypeID unit_type);
    size_t CountUnitEggs(BasicSc2Bot& bot, sc2::ABILITY_ID ability);

    // Build a drone unit
    bool BuildDrone(BasicSc2Bot& bot);

    // Build an overlord unit
    bool BuildOverlord(BasicSc2Bot& bot);

    // Build a queen unit
    bool BuildQueen(BasicSc2Bot& bot);

    bool BuildRoach(BasicSc2Bot& bot);

    bool BuildZergling(BasicSc2Bot& bot);

    void TryInjectLarva(BasicSc2Bot&bot);
    
    bool TryBuildOnCreep(BasicSc2Bot& bot, sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Point2D loc);

    bool TryExpand(BasicSc2Bot& bot, sc2::AbilityID build_ability, sc2::UnitTypeID worker_type);

    bool TryBuildStructure(BasicSc2Bot& bot, sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Point2D location, bool isExpansion);
    bool TryBuildStructure(BasicSc2Bot& bot, sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Tag location_tag);
    bool TryBuildGas(BasicSc2Bot& bot, sc2::AbilityID build_ability, sc2::UnitTypeID worker_type, sc2::Point2D base_location);

    void HandleDrones(BasicSc2Bot& bot);
    void SaturateExtractors(BasicSc2Bot& bot);

    bool TryMorphToLair(BasicSc2Bot& bot);

    bool IsQueenInTraining(BasicSc2Bot& bot);

    // Vectors of all the units we have
    // maybe can delete?

    std::vector<std::pair<sc2::Tag, bool>> bases = {};
    std::vector<std::pair<sc2::Tag, bool>> extractors = {};

private:
    const sc2::Unit* FindNearestMineralPatch(BasicSc2Bot& bot, const sc2::Point2D& start);
};

#endif