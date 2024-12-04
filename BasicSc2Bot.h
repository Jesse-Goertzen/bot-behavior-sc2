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
	virtual void OnGameEnd();

	// Each of our managers
	StateMachineManager state_machine;
	UnitManager unit_manager;

	// Unit Controllers
	Attack attack;
	bool scouted = false;
	Defend defend;

	// Inline getters for private values
	std::vector<sc2::Point3D> GetExpansions() const { return expansions_; }
	sc2::Point3D GetStartLocation() const { return startLocation_; }
	sc2::Point3D GetStagingLocation() { return staging_location_; }

	// Create a pointer to hold each expansion
	const sc2::Unit* first_expansion;

	// Create a pointer to hold each expansion
	sc2::Point3D startLocation;
    sc2::Point3D staging_location;
	std::vector<sc2::Point3D> expansions;

private:
	// From here https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.h#L124C5-L124C38
	std::vector<sc2::Point3D> expansions_;
	sc2::Point3D startLocation_;
    sc2::Point3D staging_location_;

};

#endif