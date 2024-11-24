#pragma once
#include "header_files.h"

// #include "BasicSc2Bot.h"
// Forward declaration so we dont use the whole include and have a circular dependancy
class BasicSc2Bot;

class StateMachineManager {
public:

    // Moves us to the next state
    void completeState();

    // First state
    void BuildFirstDrone(BasicSc2Bot& bot);

    void BuildFirstOverlord(BasicSc2Bot& bot);


    // States of the bot, in order of completion
	enum BotState {
        BUILD_FIRST_DRONE,
        BUILD_FIRST_OVERLORD,
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