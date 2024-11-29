#ifndef UNIT_MANAGER_H
#define UNIT_MANAGER_H

#include "header_files.h"

class BasicSc2Bot;

class UnitManager {
public:
    void OnGameStart(BasicSc2Bot& bot);

    // Return count of the unit type passed
    size_t CountUnitType(BasicSc2Bot& bot, sc2::UnitTypeID unit_type);
    // size_t CountDroneEggs(BasicSc2Bot& bot);
    size_t CountUnitEggs(BasicSc2Bot& bot, sc2::ABILITY_ID ability);

    // Build a drone unit
    bool BuildDrone(BasicSc2Bot& bot);

    // Build an overlord unit
    bool BuildOverlord(BasicSc2Bot& bot);

    // Build a queen unit
    bool BuildQueen(BasicSc2Bot& bot);

    bool BuildZergling(BasicSc2Bot& bot);

    void TryInjectLarva(BasicSc2Bot&bot);
    
    bool TryBuildOnCreep(BasicSc2Bot& bot, sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Point2D loc);

    bool TryExpand(BasicSc2Bot& bot, sc2::AbilityID build_ability, sc2::UnitTypeID worker_type);

    bool TryBuildStructure(BasicSc2Bot& bot, sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Point2D location, bool isExpansion);
    bool TryBuildStructure(BasicSc2Bot& bot, sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Tag location_tag);
    bool TryBuildGas(BasicSc2Bot& bot, sc2::AbilityID build_ability, sc2::UnitTypeID worker_type, sc2::Point2D base_location);

    // Update our list of all units
    void UpdateUnits(BasicSc2Bot& bot);

    void HandleQueenLarvae(BasicSc2Bot& bot);

    void HandleDrones(BasicSc2Bot& bot);
    void SaturateExtractors(BasicSc2Bot& bot);

    // Vectors of all the units we have
    // maybe can delete?

    std::vector<std::pair<sc2::Tag, bool>> bases = {};
    std::vector<std::pair<sc2::Tag, bool>> extractors = {};

	// sc2::Units larva;
	// sc2::Units drones;
	// sc2::Units overlords;
    // sc2::Units hatcheries;
    // sc2::Units queens;
    // sc2::Units roaches;
    // sc2::Units zerglings;
    // sc2::Units extractors;
private:
    const sc2::Unit* FindNearestMineralPatch(BasicSc2Bot& bot, const sc2::Point2D& start);
    //void UnitManager::MineIdleWorkers(BasicSc2Bot& bot, const sc2::Unit* worker, sc2::AbilityID worker_gather_command, sc2::UnitTypeID vespene_building_type);
};

#endif