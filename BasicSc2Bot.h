#ifndef BASIC_SC2_BOT_H_
#define BASIC_SC2_BOT_H_

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
#include "sc2api/sc2_unit_filters.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"

class BasicSc2Bot : public sc2::Agent {
public:
	virtual void OnGameStart();
	virtual void OnStep();
	
	bool BuildDrone();

	bool BuildOverlord();

	void UpdateUnits();

	bool TryExpand(sc2::AbilityID build_ability, sc2::UnitTypeID worker_type);

	bool TryBuildStructure(sc2::AbilityID ability_type_for_structure, sc2::UnitTypeID unit_type, sc2::Point2D location, bool isExpansion);

	// BotState NextState(BotState current);

	std::vector<const sc2::Unit*> getLarva();
	std::vector<const sc2::Unit*> getDrones();

	float getAvailableSupply();

	// State for each action
	enum BotState {
    BUILD_FIRST_DRONE,
    BUILD_OVERLORD,
    BUILD_SECOND_DRONE,
    BUILD_THIRD_DRONE,
	EXPAND,
    IDLE
	};

private:
	// sc2::Point2D GetNearestExpansion();

	// From here https://github.com/Blizzard/s2client-api/blob/614acc00abb5355e4c94a1b0279b46e9d845b7ce/examples/common/bot_examples.h#L124C5-L124C38
	std::vector<sc2::Point3D> expansions_;
	sc2::Point3D startLocation_;
    sc2::Point3D staging_location_;

	BotState current_state;

	std::vector<const sc2::Unit*> larva;

	std::vector<const sc2::Unit*> drones;

	std::vector<const sc2::Unit*> overlords;

};

#endif