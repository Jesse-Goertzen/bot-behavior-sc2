#pragma once
#include "header_files.h"

class StateMachineManager {
public:

    // Moves us to the next state
    void completeState();



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

    // What state the bot is currently in
    BotState current_state;

private:


    
};