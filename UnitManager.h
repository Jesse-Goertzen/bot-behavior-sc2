#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include "header_files.h"

class UnitManager {
public:



    // Return count of the unit type passed
    size_t CountUnitType(const sc2::ObservationInterface* observation, sc2::UnitTypeID unit_type);

    // Build a drone unit
    bool BuildDrone(const sc2::ObservationInterface* observation, sc2::ActionInterface* actions);

    // Build an overlord unit
    bool BuildOverlord(const sc2::ObservationInterface* observation, sc2::ActionInterface* actions);

    // Get a list of all larva
    std::vector<const sc2::Unit*> getLarva();

    // Update our list of all units
    void UpdateUnits(const sc2::ObservationInterface* observation);

private:

	std::vector<const sc2::Unit*> larva;

	std::vector<const sc2::Unit*> drones;

	std::vector<const sc2::Unit*> overlords;

};

#endif