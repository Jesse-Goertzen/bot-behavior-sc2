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
    std::vector<const sc2::Unit*> getLarva() { return larva; };

    // Update our list of all units
    void UpdateUnits(BasicSc2Bot& bot);

private:

    // Vectors of all the units we have
	std::vector<const sc2::Unit*> larva;
	std::vector<const sc2::Unit*> drones;
	std::vector<const sc2::Unit*> overlords;

};

#endif