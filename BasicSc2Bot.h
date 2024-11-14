#ifndef BASIC_SC2_BOT_H_
#define BASIC_SC2_BOT_H_

#include "sc2api/sc2_api.h"
#include "sc2api/sc2_args.h"
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

	// BotState NextState(BotState current);

	std::vector<const sc2::Unit*> getLarva();
	std::vector<const sc2::Unit*> getDrones();

	float getAvailableSupply();

	// State for each action
	enum BotState {
    BUILD_FIRST_DRONE,
    BUILD_OVERLORD,
    BUILD_SECOND_DRONE,
    BUILD_FIRST_HATCHERY,
    IDLE
	};

private:
	// sc2::Point2D GetNearestExpansion();



	BotState current_state;

	std::vector<const sc2::Unit*> larva;

	std::vector<const sc2::Unit*> drones;

	std::vector<const sc2::Unit*> overlords;

};

#endif