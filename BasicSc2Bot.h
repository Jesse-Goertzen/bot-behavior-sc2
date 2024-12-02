#ifndef BASIC_SC2_BOT_H_
#define BASIC_SC2_BOT_H_

#include "StateMachineManager.h"
#include "UnitManager.h"
#include "Attack.h"
#include "Defend.h"

class BasicSc2Bot : public sc2::Agent {
public:

	virtual void OnGameStart();
	virtual void OnStep();
	virtual void OnBuildingConstructionComplete(const sc2::Unit* unit);

	// Each of our managers
	StateMachineManager state_machine;
	UnitManager unit_manager;

	// Unit Controllers
	Attack attack;
	Defend defend;

	// Observation object. This is how we find everything about the game
	const sc2::ObservationInterface* observation;
	// Action object. This is how we tell units to do stuff
	sc2::ActionInterface* actions;
	// Query interface
	sc2::QueryInterface * query;

	// Inline getters for private values
	std::vector<sc2::Point3D> GetExpansions() const { return expansions; }
	sc2::Point3D GetStartLocation() const { return startLocation; }
	sc2::Point3D GetStagingLocation() { return staging_location; }

	// From here https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.h#L124C5-L124C38
	sc2::Point3D startLocation;
    sc2::Point3D staging_location;

	// Create a pointer to hold each expansion
	const sc2::Unit* first_expansion;
	std::vector<sc2::Point3D> expansions;

private:


};

#endif