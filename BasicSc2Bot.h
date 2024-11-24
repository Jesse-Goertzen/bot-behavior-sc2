#ifndef BASIC_SC2_BOT_H_
#define BASIC_SC2_BOT_H_

// Our managers
#include "StateMachineManager.h"
#include "UnitManager.h"
#include "ExpansionManager.h"


class BasicSc2Bot : public sc2::Agent {
public:

	virtual void OnGameStart();
	virtual void OnStep();

	StateMachineManager state_machine;
	UnitManager unit_manager;

	// Observation object. This is how we find everything about the game
	const sc2::ObservationInterface* observation;
	sc2::ActionInterface* actions;

	bool TryExpand(sc2::AbilityID build_ability, sc2::UnitTypeID worker_type);

	bool TryBuildStructure(sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Point2D location, bool isExpansion);

	bool TryBuildOnCreep(sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type);


	// BotState NextState(BotState current);


	size_t CountUnitType(const sc2::ObservationInterface* observation, sc2::UnitTypeID unit_type);



private:

	// sc2::Point2D GetNearestExpansion();

	// From here https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.h#L124C5-L124C38
	std::vector<sc2::Point3D> expansions_;
	sc2::Point3D startLocation_;
    sc2::Point3D staging_location_;



	// Create a pointer to hold each expansion
	const sc2::Unit* first_expansion;

};

#endif