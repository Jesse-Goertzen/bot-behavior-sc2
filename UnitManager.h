#ifndef UNIT_MANAGER_H
#define UNIT_MANAGER_H

#include "header_files.h"

class BasicSc2Bot;

class UnitManager {
public:

    // Return count of the unit type passed
    size_t CountUnitType(BasicSc2Bot& bot, sc2::UnitTypeID unit_type);

    // Build a drone unit
    bool BuildDrone(BasicSc2Bot& bot);

    // Build an overlord unit
    bool BuildOverlord(BasicSc2Bot& bot);

    // Get a list of all larva
    // std::vector<const sc2::Unit*> getLarva() { return larva; };

    // Update our list of all units
    void UpdateUnits(BasicSc2Bot& bot);

    void HandleQueenLarvae(BasicSc2Bot& bot);

    void HandleIdleDrones(BasicSc2Bot& bot);

    // Vectors of all the units we have
	sc2::Units larva;
	sc2::Units drones;
	sc2::Units overlords;
    sc2::Units hatcheries;
    sc2::Units queens;
    sc2::Units roaches;
    sc2::Units zerglings;
    sc2::Units extractors;
private:
    const sc2::Unit* FindNearestMineralPatch(BasicSc2Bot& bot, const sc2::Point2D& start);
    void UnitManager::MineIdleWorkers(BasicSc2Bot& bot, const sc2::Unit* worker, sc2::AbilityID worker_gather_command, sc2::UnitTypeID vespene_building_type);
};

#endif