#pragma once
#include "sc2api/sc2_api.h"

class StateMachineManager {
public:

    // Moves us to the next state
    void completeState();

private:

    // States of the bot, in order of completion
	enum BotState {
        BUILD_FIRST_DRONE,
        BUILD_OVERLORD,
        BUILD_SECOND_DRONE,
        BUILD_THIRD_DRONE,
        EXPAND,
        WAIT_FOR_HATCHERY,
        IDLE
	};

    // What satte the bot is on right now
    BotState current_state;
};